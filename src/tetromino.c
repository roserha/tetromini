#include "tetromino.h"
#include "sprites.h"

// Determines the x and y positions of all four blocks of a tetromino
// x:          x coordinate
// y:          y coordinate
// block_type: which block to draw
// spin_state: how the block is spinning
// positions:  8-long array of coordinates
void tetromino_get_positions(uint_fast8_t x, uint_fast8_t y, Tetromino block_type, Spin spin_state, uint8_t *positions)
{
    positions[0] = x; positions[1] = y;

    // The following approach was developed as an attempt to speed up calculating
    // shifted vectors based on a rotation matrix! The math is really really
    // unintuitive but it works kinda! Miiiight be subject to change if it turns out
    // to not actually work haha.

    // Taken from spin state
    uint_fast8_t flip = spin_state & 1;
    uint_fast8_t negate = spin_state & 2;

    // n ^ flip means we want n when not flipped and ~n when flipped
    // (1 - negate) means we want *1 when not negated and *-1 when negated
    // (-1 + negate) means we want *-1 when not negated and *1 when negated

    // Subdirections for going above central block based on spin state
    uint_fast8_t dir_up_x = (0 ^ flip) * ( 1 - negate);
    uint_fast8_t dir_up_y = (1 ^ flip) * ( 1 - negate);

    // Subdirections for going below central block based on spin state
    // these ones arent used but let's keep it here just in case
    // int8_t dir_down_x = (0 ^ flip) * (-1 + negate);
    // int8_t dir_down_y = (1 ^ flip) * (-1 + negate);

    // Subdirections for going left of central block based on spin state
    uint_fast8_t dir_left_x = (1 ^ flip) * (-1 + negate);
    uint_fast8_t dir_left_y = (0 ^ flip) * ( 1 - negate);
    
    // Subdirections for going right of central block based on spin state
    uint_fast8_t dir_right_x = (1 ^ flip) * ( 1 - negate);
    uint_fast8_t dir_right_y = (0 ^ flip) * (-1 + negate);

    switch (block_type)
    {
        case TBlock:
            positions[2] = x + dir_up_x;               positions[3] = y + dir_up_y;
            positions[4] = x + dir_left_x;             positions[5] = y + dir_left_y;
            positions[6] = x + dir_right_x;            positions[7] = y + dir_right_y;
            break;
            
        case IBlock:
            positions[2] = x + dir_left_x;                positions[3] = y + dir_left_y;
            positions[4] = x + dir_right_x;               positions[5] = y + dir_right_y;
            positions[6] = x + dir_right_x + dir_right_x; positions[7] = y + dir_right_y + dir_right_y;
            break;
            
        case OBlock:
            positions[2] = x + dir_up_x;               positions[3] = y + dir_up_y;
            positions[4] = x + dir_right_x;            positions[5] = y + dir_right_y;
            positions[6] = x + dir_up_x + dir_right_x; positions[7] = y + dir_up_y + dir_right_y;
            break;

        case SBlock:
            positions[2] = x + dir_up_x;               positions[3] = y + dir_up_y;
            positions[4] = x + dir_left_x;             positions[5] = y + dir_left_y;
            positions[6] = x + dir_up_x + dir_right_x; positions[7] = y + dir_up_y + dir_right_y;
            break;

        case ZBlock:
            positions[2] = x + dir_up_x;               positions[3] = y + dir_up_y;
            positions[4] = x + dir_right_x;            positions[5] = y + dir_right_y;
            positions[6] = x + dir_up_x + dir_left_x;  positions[7] = y + dir_up_y + dir_left_y;
            break;

        case JBlock:
            positions[2] = x + dir_left_x;             positions[3] = y + dir_left_y;
            positions[4] = x + dir_right_x;            positions[5] = y + dir_right_y;
            positions[6] = x + dir_up_x + dir_left_x;  positions[7] = y + dir_up_y + dir_left_y;
            break;

        case LBlock:
            default:
            positions[2] = x + dir_left_x;             positions[3] = y + dir_left_y;
            positions[4] = x + dir_right_x;            positions[5] = y + dir_right_y;
            positions[6] = x + dir_up_x + dir_right_x; positions[7] = y + dir_up_y + dir_right_y;
            break;

    }
}

// Draws tetrimino with the central block at (x,y) using tetrimino coordinate system
// x:          x coordinate
// y:          y coordinate
// block_type: which block to draw
// spin_state: how the block is spinning
bool tetromino_draw(uint_fast8_t x, uint_fast8_t y, Tetromino block_type, Spin spin_state)
{
    bool OutOfBounds = false;
    uint8_t Coordinates[8] = {0};

    tetromino_get_positions(x, y, block_type, spin_state, Coordinates);

    OutOfBounds |= sprite_draw_block(Coordinates[0], Coordinates[1], block_type);
    OutOfBounds |= sprite_draw_block(Coordinates[2], Coordinates[3], block_type);
    OutOfBounds |= sprite_draw_block(Coordinates[4], Coordinates[5], block_type);
    OutOfBounds |= sprite_draw_block(Coordinates[6], Coordinates[7], block_type);

    return OutOfBounds;
}

// Draws phantom tetrimino with the central block at (x,y) using tetrimino coordinate system
// x:          x coordinate
// y:          y coordinate
// block_type: which block to draw
// spin_state: how the block is spinning
bool tetromino_draw_phantom(uint_fast8_t x, uint_fast8_t y, Tetromino block_type, Spin spin_state)
{
    bool OutOfBounds = false;
    uint8_t Coordinates[8] = {0};

    tetromino_get_positions(x, y, block_type, spin_state, Coordinates);

    for (int i = 0; i < 8; i += 2)
    {
        uint_fast8_t x = Coordinates[i];
        uint_fast8_t y = Coordinates[i+1];

        if (x >= 10 || y >= 18 || block_type > 7) { OutOfBounds = true; continue; }

        sprite_draw(104, y*6 + 2, x*6 + 2);
    }

    return OutOfBounds;
}