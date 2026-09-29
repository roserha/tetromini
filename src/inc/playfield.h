#ifndef PLAYFIELD
#define PLAYFIELD

#include <stdbool.h>
#include "tetromino.h"
#include "sprites.h"
#include "graphics.h"

// Set state of block on bitfield based on tetrimino-coordinates
// x:          x coordinate
// y:          y coordinate
// block_type: which block to set state of based on the enum. 0 means empty!
void playfield_set_state(uint8_t x, uint8_t y, uint8_t block_type);

// Renders active playfield
void playfield_render();

// Renders current info (score, level, next piece)
void playfield_print_header();

// Pull a random tetromino from the bag
Tetromino playfield_get_new_piece();

// Check for collision of a hypothetical tetromino
// block_type: Which block to check a collision for
// spin_state: What spin state to check it in
// x:          What x coordinate it is in
// y:          What y coordinate it is in
// Returns true if collision happens
bool playfield_check_collision(Tetromino block_type, Spin spin_state, uint_fast8_t x, uint_fast8_t y);

// Rotate tetromino left or right
// right: true to rotate right, false to rotate left
void playfield_rotate_current_piece(bool right);

// Move tetromino right or left
// right: true to rotate right, false to rotate left
void playfield_move_current_piece(bool right);

// Soft drop piece
void playfield_soft_drop();

// Hard drop piece to phantom position
void playfield_hard_drop();

// Update phantom tetromino Y Pos
void playfield_update_phantom();

// Initializes playfield variables
void playfield_init();

// Update current playfield status
// delta_time: how much time passed since last frame
// elapsed_time: how much time passed since boot
// user_data: array of bools containing detailed user input (check main.c for documentation)
void playfield_tick(int64_t delta_time, int64_t elapsed_time, bool *user_data);

#endif