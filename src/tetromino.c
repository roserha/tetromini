#include "tetromino.h"
#include "sprites.h"
#include <stdio.h>

// Draws tetrimino with the central block at (x,y) using tetrimino coordinate system
// x:          x coordinate
// y:          y coordinate
// block_type: which block to draw
// spin_state: how the block is spinning
void tetromino_draw(uint_fast8_t x, uint_fast8_t y, Tetromino block_type, Spin spin_state)
{
    if (block_type == OBlock)
    {
        sprite_draw_block(x,y,block_type);
        sprite_draw_block(x + 1, y, block_type);
        sprite_draw_block(x, y - 1, block_type);
        sprite_draw_block(x + 1, y - 1, block_type);
        return;
    }

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
            sprite_draw_block(x,y,block_type);
            sprite_draw_block(x + dir_up_x, y + dir_up_y, block_type);
            sprite_draw_block(x + dir_left_x, y + dir_left_y, block_type);
            sprite_draw_block(x + dir_right_x, y + dir_right_y, block_type);
            break;

        case SBlock:
            sprite_draw_block(x,y,block_type);
            sprite_draw_block(x + dir_up_x, y + dir_up_y, block_type);
            sprite_draw_block(x + dir_left_x, y + dir_left_y, block_type);
            sprite_draw_block(x + dir_up_x + dir_right_x, y + dir_up_y + dir_right_y, block_type);
            break;

        case ZBlock:
            sprite_draw_block(x,y,block_type);
            sprite_draw_block(x + dir_up_x, y + dir_up_y, block_type);
            sprite_draw_block(x + dir_right_x, y + dir_right_y, block_type);
            sprite_draw_block(x + dir_up_x + dir_left_x, y + dir_up_y + dir_left_y, block_type);
            break;

        case JBlock:
            sprite_draw_block(x,y,block_type);
            sprite_draw_block(x + dir_left_x, y + dir_left_y, block_type);
            sprite_draw_block(x + dir_right_x, y + dir_right_y, block_type);
            sprite_draw_block(x + dir_up_x + dir_left_x, y + dir_up_y + dir_left_y, block_type);
            break;

        case LBlock:
            sprite_draw_block(x,y,block_type);
            sprite_draw_block(x + dir_left_x, y + dir_left_y, block_type);
            sprite_draw_block(x + dir_right_x, y + dir_right_y, block_type);
            sprite_draw_block(x + dir_up_x + dir_right_x, y + dir_up_y + dir_right_y, block_type);
            break;

        case IBlock:
            // Special rotation case: pivot between block 2 and 3
            switch (spin_state)
            {
                case ZeroDeg:
                    sprite_draw_block(x-1,y,block_type);
                    sprite_draw_block(x,y,block_type);
                    sprite_draw_block(x+1,y,block_type);
                    sprite_draw_block(x+2,y,block_type);
                    break;

                case NinetyDeg:
                    sprite_draw_block(x,y-1,block_type);
                    sprite_draw_block(x,y,block_type);
                    sprite_draw_block(x,y+1,block_type);
                    sprite_draw_block(x,y+2,block_type);
                    break;

                case OneEightyDeg:
                    sprite_draw_block(x-1,y+1,block_type);
                    sprite_draw_block(x,y+1,block_type);
                    sprite_draw_block(x+1,y+1,block_type);
                    sprite_draw_block(x+2,y+1,block_type);
                    break;

                case TwoSeventyDeg:
                default:
                    sprite_draw_block(x+1,y-1,block_type);
                    sprite_draw_block(x+1,y,block_type);
                    sprite_draw_block(x+1,y+1,block_type);
                    sprite_draw_block(x+1,y+2,block_type);
                    break;
            }
        
        default:
            break;
    }
}