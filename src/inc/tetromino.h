#ifndef TETROMINO
#define TETROMINO

#include <stdbool.h>
#include <stdint.h>

// Which tetromino is to be drawn
typedef enum {
    TBlock = 0,
    OBlock = 1,
    IBlock = 2,
    ZBlock = 3,
    SBlock = 4,
    LBlock = 5,
    JBlock = 6
} Tetromino;

// How the tetromino is spun
// Defined per bit to make spinning math easier
// based on simplified direction vector * rotation matrix math
// 1s bit means flip x and ys
// 2s bit means negate direction
typedef enum{
    ZeroDeg = 0b00,     
    NinetyDeg = 0b11,   
    OneEightyDeg = 0b10,
    TwoSeventyDeg = 0b01
} Spin;

// Draws tetrimino with the central block at (x,y) using tetrimino coordinate system
// x:          x coordinate
// y:          y coordinate
// block_type: which block to draw
// spin_state: how the block is spinning
void tetromino_draw(uint8_t x, uint8_t y, Tetromino block_type, Spin spin_state);

#endif