#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/input/input.h>
#include "graphics.h"
#include "sprites.h"
#include "tetromino.h"
#include "playfield.h"

bool user_data [1] =
{
	false, // is user softdropping
};

static void input_cb(struct input_event *evt, void *user_data)
{
    // Key press
    if (evt->type == INPUT_EV_KEY && evt->value == 1) {
        switch (evt->code) {
        case INPUT_BTN_X: // Rotate left
            playfield_rotate_current_piece(false);
            break;
        case INPUT_BTN_Y: // Rotate right
            playfield_rotate_current_piece(true);
            break;
        case INPUT_BTN_DPAD_RIGHT: // Move right
            playfield_move_current_piece(true);
            break;
        case INPUT_BTN_DPAD_LEFT: // Move left
            playfield_move_current_piece(false);
            break;
		case INPUT_BTN_DPAD_DOWN: // Begin soft drop
			((bool *)user_data)[0] = true;
			break;
		case INPUT_BTN_DPAD_UP: // Hard drop
			playfield_hard_drop();
			break;
        default:
            // LOG_INF("Unknown key code: %d", evt->code);
            break;
        }
    }

	// Key release
	if (evt->type == INPUT_EV_KEY && evt->value == 0) {
        switch (evt->code) {
		case INPUT_BTN_DPAD_DOWN: // End soft drop
			((bool *)user_data)[0] = false;
			break;
        default:
            break;
        }
    }
}

INPUT_CALLBACK_DEFINE(NULL, input_cb, user_data);

int main(void)
{
	printf("Hello World! %s\n", CONFIG_BOARD_TARGET);
	printf("Running Tetrimini!!!\n");

	int display_ready = gfx_init();
	if (display_ready == -1)
	{
		printf("display not ready :(\n");
	}
	else
	{
		printf("display ready!!!\n");
	}

	playfield_init();

	int64_t time = k_uptime_get();
	int64_t start_time = time;
	int64_t old_time = time;
	int64_t elapsed_time = 0;
	int64_t delta_math_time = 0;
	int64_t delta_frame_time = 0;

	while (true)
	{
		// Delta time calculations
		old_time = time;
		elapsed_time = time - start_time;

		// Override frame
		gfx_clear();
		
		// Draw current playfield
		playfield_render();

		// Draw header info
		playfield_print_header();

		// Update playfield
		playfield_tick(delta_frame_time, elapsed_time, user_data);
		
		// Update delta_time and wait for next framedraw time
		time = k_uptime_get();
		delta_math_time = time - old_time;
		int64_t frame_sleep_time = 17 - delta_math_time;
		
		if (frame_sleep_time > 0)
		{
			k_timeout_t timeout = K_MSEC(1);
			k_sleep(timeout);
		}
		
		// Refresh screen and wait for next frame (if we finished early)
		gfx_flush();
		time = k_uptime_get();
		delta_frame_time = time - old_time;
	}

	return 0;
}
