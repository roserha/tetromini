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

	gfx_playfield();

	// t block
	tetromino_draw(4, 3, TBlock, ZeroDeg);

	// full line
	for (int i = 0; i < 10; i++)
	{
		playfield_set_state(i, 0, i % 7, true);
	}

	playfield_update_occupied();

	// text test
	sprite_draw_text(121, 1, "Score: 12345678");
	sprite_draw_text(113, 1, "Lvl: 123");
	sprite_draw_text(113, 34, "Next: #");

	int64_t time = k_uptime_get();
	int64_t start_time = time;
	int64_t old_time = time;
	int16_t delta_time = 0;

	while (true)
	{
		// Delta time calculations
		old_time = time;
		
		// Draw current playfield
		playfield_render();

		// Debug spinning piece

		int64_t rotate = (time - start_time) / 250;
		Spin spins[4] = {ZeroDeg, NinetyDeg, OneEightyDeg, TwoSeventyDeg};
		Spin rotate_dir = spins[rotate % 4];
		tetromino_draw(4, 3, (rotate >> 2) % 7, rotate_dir);
		
		time = k_uptime_get();
		delta_time = time - old_time;
		int16_t frame_sleep_time = 17 - delta_time;
		
		if (frame_sleep_time > 0)
		{
			k_sleep(K_MSEC(frame_sleep_time)); // Sleep for ~60hz
		}
		
		// Refresh screen and wait for next frame (if we finished early)
		gfx_flush();
	}

	return 0;
}
