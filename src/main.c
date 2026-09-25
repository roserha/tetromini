#include <stdio.h>
#include <zephyr/kernel.h>
#include "graphics.h"
#include "sprites.h"
#include "tetromino.h"
#include "playfield.h"

int main(void)
{
	printf("Hello World! %s\n", CONFIG_BOARD_TARGET);
	printf("Running Tetrimini!!!\n");

	gfx_init();
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

		// Draw header info
		playfield_print_header();
		
		// Draw current playfield
		playfield_render();

		// Update playfield
		playfield_tick(delta_frame_time, elapsed_time);
		
		// Update delta_time and wait for next framedraw time
		time = k_uptime_get();
		delta_math_time = time - old_time;
		int64_t frame_sleep_time = 17 - delta_math_time;
		
		if (frame_sleep_time > 0)
		{
			k_timeout_t timeout = K_MSEC(frame_sleep_time);
			k_sleep(timeout);
		}
		
		// Refresh screen and wait for next frame (if we finished early)
		gfx_flush();
		time = k_uptime_get();
		delta_frame_time = time - old_time;
	}

	return 0;
}
