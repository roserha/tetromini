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

// Renders current info (score, level, next piece)
void playfield_print_header();

// Pull a random tetromino from the bag
Tetromino playfield_get_new_piece();

// Initializes playfield variables
void playfield_init();

// Check for collision of a hypothetical tetromino
// Returns true if collision happens
bool playfield_check_collision(Tetromino block_type, Spin spin_state, uint_fast8_t x, uint_fast8_t y);

// Update current playfield status
void playfield_tick(int64_t delta_time, int64_t elapsed_time);

#endif