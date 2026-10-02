#include <stdio.h>                 // Printfs
#include <zephyr/kernel.h>         // Timeouts, message queues, sleep
#include <zephyr/input/input.h>    // Input callbacks
#include <zephyr/device.h>         // Voltage monitor logic
#include <zephyr/drivers/sensor.h> // Ditto
#include <hal/nrf_gpio.h>          // System power off and on
#include <zephyr/sys/poweroff.h>   // Ditto
#include <zephyr/drivers/gpio.h>   // Ditto 2
#include "graphics.h"
#include "sprites.h"
#include "tetromino.h"
#include "playfield.h"
#include "inputcodes.h"

typedef enum 
{
	screenMainMenu,
	screenGame,
	screenLowBattery,
	screenShutDown
} screen_state;

screen_state current_screen = screenMainMenu;

///////////////////////////////////////////////////////////////////////////
// INPUT CALLBACK HANDLING
///////////////////////////////////////////////////////////////////////////
// Here we define some message queue variables that are needed
///////////////////////////////////////////////////////////////////////////

char input_msgq_buffer[16 * sizeof(uint8_t)];
struct k_msgq input_msgq;

static void input_cb(struct input_event *evt, void *user_data)
{
	// Input callback elements are determined as follows, from least to most significant bit:
	// 1st bit is whether it was a key press or key release
	// 2nd, 3rd and 4th bit identify which key was pressed
	// Last bit is set to 1 to differentiate between no messages and some messages.

	uint8_t event = 0b10000000;
	uint8_t event_state = (evt->value == 1) ? 0b0 : 0b1;
	uint8_t event_key = 0b000;

	switch (evt->code) {
        case INPUT_BTN_X: // Rotate left
            event_key = 0b000;
            break;
        case INPUT_BTN_Y: // Rotate right
            event_key = 0b001;
            break;
        case INPUT_BTN_DPAD_RIGHT: // Move right
            event_key = 0b010;
            break;
        case INPUT_BTN_DPAD_LEFT: // Move left
            event_key = 0b011;
            break;
		case INPUT_BTN_DPAD_DOWN: // Begin soft drop
			event_key = 0b100;
			break;
		case INPUT_BTN_DPAD_UP: // Hard drop
			event_key = 0b101;
			break;
		case INPUT_BTN_START:
			event_key = 0b110;
			break;
		case INPUT_KEY_POWER:
		default:
			event_key = 0b111;
            break;
	}

	event |= event_state | (event_key << 1);

	while (k_msgq_put(&input_msgq, &event, K_NO_WAIT) != 0)
	{
		k_msgq_purge(&input_msgq);
	}
}

// Pop oldest message on a first-in first out system, and
// automatically shift remaining messages
// received_buf: Buffer of received messages
// buf_len: Length of buffer
// Returns most recent element if there are any
// or 0 if there are no more elements
uint8_t fetchMessage(uint8_t* received_buf, int *buf_len)
{
	if (*buf_len == 0)
	{
		return 0;
	}

	uint8_t retNum = received_buf[0];

	memmove(&received_buf[0], &received_buf[1], sizeof(uint8_t) * (*buf_len - 1));
	received_buf[*buf_len - 1] = 0;
	*buf_len = *buf_len - 1;

	return retNum;
}


///////////////////////////////////////////////////////////////////////////
// BATTERY VOLTAGE HANDLING
///////////////////////////////////////////////////////////////////////////
// Here we define mutexes and thread entry points for the voltage_sensor
///////////////////////////////////////////////////////////////////////////

#define BATTERY_THREAD_STACK_SIZE 500
#define BATTERY_THREAD_PRIORITY 5

// Battery voltages to indexed "perdec", taken from battery.h
static const int64_t battery_mv_percentages[11] = {
	3305, 3686, 3741, 3775, 3793, 3820,
	3884, 3945, 4008, 4085, 4177
};

// Battery millivoltage mutex
struct k_mutex battery_mv_mutex;
int64_t battery_mv;
uint_fast8_t battery_percent;

// Battery Level Thread Logic
extern void battery_level_thread_entrypoint(void *mv_mutex_void, void *mv_void, void * percent_void)
{
	// Convert void pointers to type-specific pointers
	struct k_mutex *mv_mutex = (struct k_mutex *) mv_mutex_void;
	int64_t *mv = (int64_t *)mv_void;
	uint_fast8_t *percent = (uint_fast8_t *)percent_void;

	/* the board dtsi defines a node at /vbatt with compatible "voltage-divider" */
	static const struct device *const vbatt = DEVICE_DT_GET(DT_PATH(vbatt));

	// Prepare voltage divider for battery voltage
	if (!device_is_ready(vbatt)) {
		printk("vbatt not ready (missing adc channel@5 in overlay?)\n");
		return;
	}

	// Permanent loop to query battery level
	while (true)
	{
		int64_t previous_mv = 0;
		uint_fast8_t previous_percentage = 0;
		int64_t new_mv = 0;
		uint_fast8_t new_percentage = 0;

		int mutex_state = k_mutex_lock(mv_mutex, K_MSEC(100));
		if (mutex_state == 0)
		{
			// Successfully locked mutex, let's average 5 readings
			previous_mv = *mv;
			previous_percentage = *percent;
			k_mutex_unlock(mv_mutex);
		}

		int successful_readings = 0;

		for (int i = 0; successful_readings < 10 && i < 20; i++)
		{
			struct sensor_value val;
			int err = sensor_sample_fetch(vbatt);

			if (!err)
			{
				err = sensor_channel_get(vbatt, SENSOR_CHAN_VOLTAGE, &val);
				if (!err) 
				{
					new_mv += sensor_value_to_milli(&val);
					successful_readings++;
				}
				else
				{
					perror("err obtaining sensor channel");
				}
			}
			else
			{
				perror("err obtaining sensor value");
			}

			k_sleep(K_MSEC(750));
		}

		if (successful_readings > 0)
		{
			new_mv /= successful_readings;
			
			if (previous_mv != 0)
			{
				new_mv = (previous_mv + new_mv) / 2;
			}
		} else 
		{
			new_mv = previous_mv;
		}

		int lower_bound_idx = -1; int64_t lower_bound = 0;
		int upper_bound_idx = -1; int64_t upper_bound = 0;

		// Get lower and upper bounds
		for (int i = 0; i <= 10; i++)
		{
			if (battery_mv_percentages[i] < new_mv)
			{
				lower_bound_idx = i;
				lower_bound = battery_mv_percentages[i];
			}
			else
			{
				upper_bound_idx = i;
				upper_bound = battery_mv_percentages[i];
				break;
			}
		}
		// Edge cases: 
		// new_mv < 0% will have lower_bound = -1 because no lower_bound exists
		if (lower_bound_idx == -1)
		{
			new_percentage = 0;
		}
		// new_mv > 100% will have upper_bound = -1 because no upper_bound exists
		else if (upper_bound_idx == -1)
		{
			new_percentage = 100;
		}
		// if we have lower AND upper bound, find percentage using geometric distance
		else
		{
			int64_t bounds = upper_bound - lower_bound;
			int64_t distance = (new_mv - lower_bound) * 10;

			new_percentage = lower_bound_idx * 10 + (distance/bounds);
		}

		if (previous_percentage != 0)
		{
			new_percentage = (previous_percentage + new_percentage) / 2;
		}

		mutex_state = k_mutex_lock(mv_mutex, K_MSEC(100));
		if (mutex_state == 0)
		{
			*mv = new_mv;
			*percent = new_percentage;
			k_mutex_unlock(mv_mutex);
		}

		k_sleep(K_SECONDS(5));
	}
}

// Get current battery level, waiting at most 2ms for mutex to be unlocked
// Returns current battery level in mv if successful, 0 otherwise.
int64_t get_battery_level(struct k_mutex *mv_mutex, int64_t *mv)
{
	int64_t mv_ret = 0;

	if (k_mutex_lock(mv_mutex, K_MSEC(2)) == 0)
	{
		mv_ret = *mv;
		k_mutex_unlock(mv_mutex);
	}

	return mv_ret;
}

// Get current battery percentage, waiting at most 2ms for mutex to be unlocked
// Returns current battery percentage if successful, 255 otherwise.
uint_fast8_t get_battery_percentage(struct k_mutex *mv_mutex, uint_fast8_t*percentage)
{
	uint_fast8_t mv_ret = 255;

	if (k_mutex_lock(mv_mutex, K_MSEC(2)) == 0)
	{
		mv_ret = *percentage;
		k_mutex_unlock(mv_mutex);
	}

	return mv_ret;
}

K_THREAD_STACK_DEFINE(battery_stack_area, BATTERY_THREAD_STACK_SIZE);
struct k_thread battery_level_thread_data;

///////////////////////////////////////////////////////////////////////////
// POWERING ON AND OFF
///////////////////////////////////////////////////////////////////////////
// This is where we handle powering the system on and off
///////////////////////////////////////////////////////////////////////////

#define WAKEUP_PIN_NUMBER 3 // A5
int64_t time_since_power_button_lifted = 0; // If it's been 1500ms since it's been let go, turn off

void trigger_poweroff(void)
{
	printk("Turning Tetromino off. Press A5 to turn it back on.");

	nrf_gpio_cfg(
		WAKEUP_PIN_NUMBER,
		NRF_GPIO_PIN_DIR_INPUT,
		NRF_GPIO_PIN_INPUT_CONNECT,
		NRF_GPIO_PIN_PULLDOWN,
		NRF_GPIO_PIN_SENSE_HIGH,
		NRF_GPIO_PIN_S0S1
	);

	printk("Shutting down.");

	k_sleep(K_MSEC(100));

	sys_poweroff();
}

///////////////////////////////////////////////////////////////////////////
// MAIN FIRMWARE LOOP
///////////////////////////////////////////////////////////////////////////
// Here we actually do our loop! Yay!!!
///////////////////////////////////////////////////////////////////////////

bool user_data [3] =
{
	false, // is user softdropping
	false, // is user holding powerdown button
	false, // ready to power off
};

INPUT_CALLBACK_DEFINE(NULL, input_cb, user_data);

int main(void)
{
	printf("Hello World! %s\n", CONFIG_BOARD_TARGET);
	
	printf("Creating message queue and mutex.\n");

	// Starting input message queue ring buffer
	k_msgq_init(&input_msgq, input_msgq_buffer, sizeof(uint8_t), 16);

	// Declaring battery millivoltage mutex
	k_mutex_init(&battery_mv_mutex);

	printf("Starting battery thread.\n");

	// Starting battery thread
	k_tid_t battery_level_tid = k_thread_create(&battery_level_thread_data, battery_stack_area,
											K_THREAD_STACK_SIZEOF(battery_stack_area),
											battery_level_thread_entrypoint,
											&battery_mv_mutex, &battery_mv, &battery_percent,
											BATTERY_THREAD_PRIORITY, 0, K_NO_WAIT);

	// Prepare display
	int display_ready = gfx_init();
	if (display_ready == -1)
	{
		printf("display not ready :(\n");
	}
	else
	{
		printf("display ready!!!\n");
	}


	// Create time management variables

	// Time since boot at the start of the frame
	int64_t frame_start_time = k_uptime_get();
	// Time since boot at the start of the previous frame
	int64_t last_frame_start_time = frame_start_time;
	int64_t delta_frame_time = 0;

	// Buffer received from the message queue in single frame
	uint8_t received_msgq_buffer[16] = {0};
	// How many messages are contained in the buffer
	int numMessagesReceived = 0;
	// Most recent messsage from buffer
	uint8_t recent_msg = 0;

	printf("Entering main loop.\n");
	while (true)
	{
		// Delta time calculations
		last_frame_start_time = frame_start_time;
		frame_start_time = k_uptime_get();
		delta_frame_time = frame_start_time - last_frame_start_time;

		// Check if we should power off system
		if (!user_data[1])
		{
			// Increase timer if we aren't pressing button
			time_since_power_button_lifted = frame_start_time;

			// Shut down if we have held button and since let go
			if (user_data[2])
			{
				gfx_shutdown();
				trigger_poweroff();
			}
		}

		// If we have held down power button, time to turn things off!
		if (frame_start_time - time_since_power_button_lifted > 1500)
		{
			user_data[2] = true;
			current_screen = screenShutDown;
		}

		// Battery info
		int64_t current_mv_64 = get_battery_level(&battery_mv_mutex, &battery_mv);
		int32_t current_mv = 0;
		if (current_mv_64 <= INT32_MAX && current_mv_64 >= INT32_MIN)
		{
			current_mv = (int32_t) current_mv_64;
		}

		uint_fast8_t current_percentage = get_battery_percentage(&battery_mv_mutex, &battery_percent);

		// Override frame
		gfx_clear();

		// Request input events from message queue
		while (k_msgq_get(&input_msgq, &(received_msgq_buffer[numMessagesReceived]), K_NO_WAIT) == 0 && numMessagesReceived < 16)
		{
			numMessagesReceived++;
		}

		// Show current screen and process inputs
		switch (current_screen)
		{
			case screenMainMenu:
				// Draw menu header
				sprite_draw_text(121,1, "TETROMINI");
				sprite_draw_text(113,1, "Press START");
				sprite_draw_text(105,1, "to play!");
				sprite_draw_text(0,1, "(c) Rose");

				sprite_draw_text(70,
					sprite_draw_number(70,
						sprite_draw_text(70,1, "Charge:"), 
					current_mv, 10),
				"mV");

				sprite_draw_text(62,
					sprite_draw_number(62,
						sprite_draw_text(62,1, "Level: "), 
					current_percentage, 10),
				"%");

				recent_msg = fetchMessage(received_msgq_buffer, &numMessagesReceived);
				while (recent_msg != 0)
				{
					switch (recent_msg)
					{
						case TTO_START_RELEASE: // Let go of start key
							playfield_init();
							current_screen = screenGame;
							recent_msg = 0;
							break;

						case TTO_POWER_PRESS:
							user_data[1] = true;
							break;

						case TTO_POWER_RELEASE:
							user_data[1] = false;
							break;
						
						default:
							break;
					}
					
					if (current_screen == screenMainMenu)
					{
						recent_msg = fetchMessage(received_msgq_buffer, &numMessagesReceived);
					}
				}
				break;

			case screenGame:
				recent_msg = fetchMessage(received_msgq_buffer, &numMessagesReceived);
				while (recent_msg != 0)
				{
					switch (recent_msg)
					{
						case TTO_ROT_LEFT_PRESS:
							playfield_rotate_current_piece(false);
							break;
						
						case TTO_ROT_RIGHT_PRESS:
							playfield_rotate_current_piece(true);
							break;

						case TTO_MOVE_LEFT_PRESS:
							playfield_move_current_piece(false);
							break;

						case TTO_MOVE_RIGHT_PRESS:
							playfield_move_current_piece(true);
							break;

						case TTO_HARD_DROP_PRESS:
							playfield_hard_drop();
							break;

						case TTO_SOFT_DROP_PRESS:
							user_data[0] = true;
							break;

						case TTO_SOFT_DROP_RELEASE:
							user_data[0] = false;
							break;

						case TTO_POWER_PRESS:
							user_data[1] = true;
							break;

						case TTO_POWER_RELEASE:
							user_data[1] = false;
							break;

						default:
							break;
					}
					
					if (current_screen == screenGame)
					{
						recent_msg = fetchMessage(received_msgq_buffer, &numMessagesReceived);
					}
				}

				// Update playfield
				playfield_tick(delta_frame_time, frame_start_time, user_data);

				// Draw current playfield
				playfield_render();

				// Draw header info
				playfield_print_header();
				break;

			case screenLowBattery:
			break;
			
			case screenShutDown:
			default:
				sprite_draw_text(121,1, "Thanks for");
				sprite_draw_text(113,1, "playing! Let");
				sprite_draw_text(105,1, "go to shutdown.");

				recent_msg = fetchMessage(received_msgq_buffer, &numMessagesReceived);
				while (recent_msg != 0)
				{
					switch (recent_msg)
					{
						case TTO_POWER_RELEASE:
							user_data[1] = false;
							break;
						
						default:
							break;
					}
					
					recent_msg = fetchMessage(received_msgq_buffer, &numMessagesReceived);
				}
				break;
			break;
		}
		
		// Draw display
		gfx_flush();

		// Wait until 17ms (~60Hz) have passed since the start of the frame
		k_timeout_t timeout_time = K_TIMEOUT_ABS_MS(frame_start_time + 17);
		k_sleep(timeout_time);
	}

	return 0;
}
