#ifndef PLAYFIELD
#define PLAYFIELD

#include <stdbool.h>
#include "tetromino.h"
#include "sprites.h"
#include "graphics.h"

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