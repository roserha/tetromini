// This file contains programatically generated content via sprite_factory.py.
// Whatever you write inside there will be overriden when executing the program.

#ifndef SPRITES
#define SPRITES

// v THE CONTENTS BELOW WERE PROGRAMATICALLY GENERATED v //
///////////////////////////////////////////////////////////

#define SPRITE_NUM 104
// Storage taken by sprites:                728B

///////////////////////////////////////////////////////////
// ^ THE CONTENTS ABOVE WERE PROGRAMATICALLY GENERATED ^ //

// Previously, I did multiple sprite arrays with pointers stored for each of them.
// That was bad, because although the sprite data only took up 533B, the pointer
// array took up a massive 3.328KB.
// For 104 sprites, dynamic sized arrays are 533B, and static-sized array is 728B.
// So yeah, I'll do my own O(1) lookup based on array size instead of pointer actions.

#include <stdbool.h>
#include <stdint.h>

// Draws sprite based on sprite id defined in rsc/sprites.bmp at specified coordinate using transposed coordinate system
// sprite_id: Which sprite to draw
// x:  x coordinate
// y:  y coordinate
//
// Returns horizontal resolution of the drawn sprite
uint8_t sprite_draw(uint8_t sprite_id, uint8_t x, uint8_t y);

// Draws 6px-wide block using tetrimino coordinate system
// x:  x coordinate
// y:  y coordinate
// block_type: which block to draw based on block enum ids
void sprite_draw_block(int8_t x, int8_t y, uint8_t block_type);

// Writes text using sprites at specified coordinate using transposed coordinate system
// x:        x coordinate
// y:        y coordinate
// sentence: *null-terminated* string
void sprite_draw_text(uint8_t x, uint8_t y, char *sentence);
#endif