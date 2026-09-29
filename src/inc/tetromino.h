#ifndef TETROMINO
#define TETROMINO

#include <stdbool.h>
#include <stdint.h>

// Which tetromino is to be drawn
typedef enum {
    TBlock = 1,
    OBlock = 2,
    IBlock = 3,
    ZBlock = 4,
    SBlock = 5,
    LBlock = 6,
    JBlock = 7
} Tetromino;

// How the tetromino is spun
// Defined per bit to make spinning math easier
// based on simplified direction vector * rotation matrix math
// 1s bit means flip x and ys
// 2s bit means negate direction
typedef enum{
    ZeroDeg = 0b00,     
    TwoSeventyDeg = 0b01,
    OneEightyDeg = 0b10,
    NinetyDeg = 0b11
} Spin;

// Determines the x and y positions of all four blocks of a tetromino
// x:          x coordinate
// y:          y coordinate
// block_type: which block to draw
// spin_state: how the block is spinning
// positions:  8-long array of coordinates
void tetromino_get_positions(uint_fast8_t x, uint_fast8_t y, Tetromino block_type, Spin spin_state, uint8_t *positions);

// Draws tetrimino with the central block at (x,y) using tetrimino coordinate system
// x:          x coordinate
// y:          y coordinate
// block_type: which block to draw
// spin_state: how the block is spinning
// Returns false if a block was out of bounds
bool tetromino_draw(uint_fast8_t x, uint_fast8_t y, Tetromino block_type, Spin spin_state);

// Draws phantom tetrimino with the central block at (x,y) using tetrimino coordinate system
// x:          x coordinate
// y:          y coordinate
// block_type: which block to draw
// spin_state: how the block is spinning
bool tetromino_draw_phantom(uint_fast8_t x, uint_fast8_t y, Tetromino block_type, Spin spin_state);

#endif