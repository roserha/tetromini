#include "tetromino.hpp"

std::array<uint8_t, 8> TetrominoController::getTetrominoPositions(uint_fast8_t x, uint_fast8_t y, Tetromino_t block_type, Spin_t spin_state)
{
    std::array<uint8_t, 8> positions = {0};

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

    return positions;
}

bool TetrominoController::drawTetromino(uint_fast8_t x, uint_fast8_t y, Tetromino_t block_type, Spin_t spin_state)
{
    bool OutOfBounds = false;
    std::array<uint8_t, 8> Coordinates = getTetrominoPositions(x, y, block_type, spin_state);

    OutOfBounds |= _sprites_controller->drawBlock(Coordinates[0], Coordinates[1], block_type);
    OutOfBounds |= _sprites_controller->drawBlock(Coordinates[2], Coordinates[3], block_type);
    OutOfBounds |= _sprites_controller->drawBlock(Coordinates[4], Coordinates[5], block_type);
    OutOfBounds |= _sprites_controller->drawBlock(Coordinates[6], Coordinates[7], block_type);

    return OutOfBounds;
}

bool TetrominoController::drawPhantomTetromino(uint_fast8_t x, uint_fast8_t y, Tetromino_t block_type, Spin_t spin_state)
{
    bool OutOfBounds = false;
    std::array<uint8_t, 8> Coordinates = getTetrominoPositions(x, y, block_type, spin_state);

    for (int i = 0; i < 8; i += 2)
    {
        uint_fast8_t x = Coordinates[i];
        uint_fast8_t y = Coordinates[i+1];

        if (x >= 10 || y >= 18 || block_type > 7) { OutOfBounds = true; continue; }

        _sprites_controller->drawSprite(104, y*6 + 2, x*6 + 2);
    }

    return OutOfBounds;
}