#ifndef PLAYFIELD
#define PLAYFIELD

#include <stdbool.h>
#include "tetromino.h"
#include "sprites.h"
#include "graphics.h"

// We will keep track of the active playfield by borrowing bitboards from chess!!

// Given the playfield is 10x22, with only the first 18 rows visible, we need 220 bits for that. 
// With 8bit numbers, each piece will be in a 28-integer-long array, and we'll be wasting just 4 bits per field.
// Each piece type will have its own bitfield array, so we can easily color blocks in and determine intersections!

// With seven pieces, alongside an ORred bitfield of filled spots, we have 224B of data for playfields. Not too shabby!!

// Set state of block on bitfield based on tetrimino-coordinates
// x:          x coordinate
// y:          y coordinate
// block_type: which block to set state of
// state:      whether to turn block on or off
void playfield_set_state(uint8_t x, uint8_t y, Tetromino block_type, bool state);


// Updates status of occupied playfield
void playfield_update_occupied();

// Renders active playfield
void playfield_render();

#endif