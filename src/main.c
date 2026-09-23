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

	// full line
	for (int i = 0; i < 10; i++)
	{
		playfield_set_state(i, 0, i % 7, true);
	}

	playfield_update_occupied();

	int64_t time = k_uptime_get();
	int64_t start_time = time;
	int64_t old_time = time;
	int64_t delta_time = 0;

	while (true)
	{
		// Delta time calculations
		old_time = time;

		// Override frame
		gfx_clear();

		// Draw header info
		playfield_print_header();
		
		// Draw current playfield
		playfield_render();

		// Debug spinning piece

		int64_t rotate = (time - start_time) / 250;
		Spin spins[4] = {ZeroDeg, NinetyDeg, OneEightyDeg, TwoSeventyDeg};
		Spin rotate_dir = spins[rotate % 4];
		tetromino_draw(4, 3, (rotate >> 2) % 7, rotate_dir);
		
		// Update delta_time and wait for next framedraw time
		time = k_uptime_get();
		delta_time = time - old_time;
		int64_t frame_sleep_time = 17 - delta_time;
		
		if (frame_sleep_time > 0)
		{
			k_timeout_t timeout = K_MSEC(frame_sleep_time);
			k_sleep(timeout); // Sleep for ~60hz
		}
		
		// Refresh screen and wait for next frame (if we finished early)
		gfx_flush();
	}

	return 0;
}
