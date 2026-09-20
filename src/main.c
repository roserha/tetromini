#include <stdio.h>
#include "graphics.h"
#include "sprites.h"

int main(void)
{
	printf("Hello World! %s\n", CONFIG_BOARD_TARGET);
	printf("Running Tetrimini!!!\n");

	gfx_init();

	gfx_playfield();

	// t block
	sprite_draw_block(3, 3, 0);
	sprite_draw_block(4, 3, 0);
	sprite_draw_block(5, 3, 0);
	sprite_draw_block(4, 2, 0);

	// full line
	for (int i = 0; i < 10; i++)
	{
		sprite_draw_block(i, 17, i % 7);
	}

	// text test
	sprite_draw_text(121, 1, "Score: 12345678");
	sprite_draw_text(113, 1, "Lvl: 123");
	sprite_draw_text(113, 34, "Next: #");

	gfx_flush();

	return 0;
}
