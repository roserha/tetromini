#include "playfield.h"
#include <stdio.h>
#include <zephyr/random/random.h>

// We will keep track of the active playfield by borrowing bitboards from chess!!

// Given the playfield is 10x22, with only the first 18 rows visible, we need to store 220 blocks of data. 
// (row 0 needs to exist because otherwise floor collisions have the chance to underflow and undefined things happen :p sue me but not really)
// With 6 blocks, we can store a tile's blocktype, and naturally b000 means empty!
// With 10 tiles per row, this means 30 bits per row! Each row can be represented by a 32-bit int, wasting only 2 bits per row!
// As such, with 22 rows, our entire playfield takes up only 88 bytes!! RAH!!!!! EFFICIENCY!!!!!!!!!

uint32_t Playfield [22] = {0};
uint32_t Score = 0;
uint_fast8_t Level = 1;
uint32_t TotalLinesCleared = 0;
uint32_t Combo = 0;
uint32_t Timeout = 300;
uint_fast8_t seen_pieces = 0;
Tetromino nextPiece = TBlock;
Tetromino currentPiece = TBlock;
Spin currentSpinState = ZeroDeg;
uint_fast8_t pieceXPos = 7;
uint_fast8_t pieceYPos = 20;
uint_fast8_t phantomYPos = 20;
int64_t stopwatch = 0;

// Piece offset data
// This comes from the SRS page in the Tetris Wiki.
// The way pieces are represented made it such that the naturally best approach to deal with rotations
// and wall-kicks are by using offsets, since their rotations are naturally True Rotations, given their
// root on vector and rotational matrix multiplication!

// Sadly, we have to store them all explicitly to make computation easier...
int8_t JLSTZ_Offsets[40] = {
   0, 0,    0, 0,    0, 0,    0, 0,    0, 0,
   0, 0,    1, 0,    1,-1,    0, 2,    1, 2,
   0, 0,    0, 0,    0, 0,    0, 0,    0, 0,
   0, 0,   -1, 0,   -1,-1,    0, 2,   -1, 2
};

int8_t I_Offsets[40] = {
    0, 0,   -1, 0,    2, 0,   -1, 0,    2, 0,
   -1, 0,    0, 0,    0, 0,    0, 1,    0,-2,
   -1, 1,    1, 1,   -2, 1,    1, 0,   -2, 0,
    0, 1,    0, 1,    0, 1,    0,-1,    0, 2
};

// These values are either false for -1 and true for 0 (bc i can just do O_Offset[i] + 1)
// This is my way to at least save SOME space.
bool O_Offsets[8] = {
    true, true,
    true,false,
   false,false,
   false, true
};

bool checkForLineClear = false;
bool finishedCheck = false;
uint_fast8_t indexesToShift[4] = {0};

// Set state of block on bitfield based on tetrimino-coordinates
// x:          x coordinate
// y:          y coordinate
// block_type: which block to set state of based on the enum. 0 means empty!
void playfield_set_state(uint8_t x, uint8_t y, uint8_t block_type)
{
    if (x >= 10 || y >= 22 || block_type > 7) { return; }

    uint32_t newValue = block_type << (x * 3);
    uint32_t tileMask = ~(0b111 << (x * 3));
    Playfield[y] = (Playfield[y] & tileMask) + newValue;
}

// Renders active playfield
void playfield_render()
{
    // Draw playfield bounds
    gfx_playfield();

    // Draw playfield itself
    for (size_t i = 0; i < 18; i++) // Last few rows are out of view, so we don't need to go through all of them!
    {
        uint32_t PlayfieldRow = Playfield[i+1];
        for (size_t j = 0; j < 10; j++) 
        {
            uint32_t tileMask = (0b111 << (j * 3));
            uint32_t tileData = (PlayfieldRow & tileMask) >> (j * 3);
            uint_fast8_t blockStyle = tileData & 0b111;
            sprite_draw_block(j, i, blockStyle);
        }
    }
}

// Renders current info (score, level, next piece)
void playfield_print_header()
{
    sprite_draw_text(121, 1, "Score:");
    sprite_draw_number(121, 29, Score, 10);
	sprite_draw_text(113, 1, "Lvl:");
    sprite_draw_number(113, 18, Level, 10);
	sprite_draw_text(113, 34, "Next:");
    sprite_draw(70 + ((int)nextPiece), 113, 57);
}

// Pull a random tetromino from the bag
Tetromino playfield_get_new_piece()
{
    bool foundPiece = false;
    Tetromino piece = TBlock;

    if (seen_pieces == 0b1111111)
    {
        // We've seen all pieces, let's refresh bag
        seen_pieces = 0;
    }

    while (!foundPiece)
    {
        uint_fast8_t rand = sys_rand8_get() & 0b1111111;

        // Filter out pieces we've seen
        rand &= ~seen_pieces;

        if (rand != 0 && rand != 0b10000000)
        {
            for (int i = 0; i < 8; i++)
            {
                uint_fast8_t pieceCheckIndex = 1U << i;
                if ((rand & pieceCheckIndex) != 0)
                {
                    foundPiece = true;
                    piece = (Tetromino)(i+1);
                    seen_pieces |= pieceCheckIndex;
                    break;
                }
            }
        }
    }

    return piece;
}

// Check for collision of a hypothetical tetromino
// block_type: Which block to check a collision for
// spin_state: What spin state to check it in
// x:          What x coordinate it is in
// y:          What y coordinate it is in
// Returns true if collision happens
bool playfield_check_collision(Tetromino block_type, Spin spin_state, uint_fast8_t x, uint_fast8_t y)
{
    uint8_t Coordinates[8] = {0};
    
    tetromino_get_positions(x, y, block_type, spin_state, Coordinates);

    // Simpler collision: colliding with walls (horizontally or bottom)
    for (int i = 0; i < 8; i++)
    {
        if ((i & 1) == 0) // Checking x (i is even)
        {
            if (Coordinates[i] >= 10)
            {
                return true;
            }
        }
        else // Checking y (i is odd)
        {
            if (Coordinates[i] == 0)
            {
                return true;
            }
        }
    }

    // Harder collision: colliding with existing playfields
    // Transform x and y array into array coordinates

    for (int i = 0; i < 8; i+=2)
    {
        // Check if coordinate we're looking at is set to occupied

        size_t y = Coordinates[i+1];
        uint8_t x = Coordinates[i];

        uint32_t rowToCheck = Playfield[y];
        uint32_t tileMask = (0b111 << (x * 3));

        if ((rowToCheck & tileMask) != 0)
        {
            return true;
        }
    }

    // No collisions here!
    return false;
}

// Rotate tetromino left or right
// right: true to rotate right, false to rotate left
void playfield_rotate_current_piece(bool right)
{
    Spin desiredSpinState = currentSpinState;

    switch (currentSpinState)
    {
        case ZeroDeg:
            desiredSpinState = right ? TwoSeventyDeg : NinetyDeg;
            break;

        case TwoSeventyDeg:
            desiredSpinState = right ? OneEightyDeg : ZeroDeg;
            break;

        case OneEightyDeg:
            desiredSpinState = right ? NinetyDeg : TwoSeventyDeg;
            break;

        default:
        case NinetyDeg:
            desiredSpinState = right ? ZeroDeg : OneEightyDeg;
            break;
    }

    // Wall kick detection logic
    if (currentPiece == OBlock)
    {
        int_fast8_t targXKick = O_Offsets[desiredSpinState*2 + 0] + 1; int_fast8_t targYKick = O_Offsets[desiredSpinState*2 + 1] + 1;
        int_fast8_t prevXKick = O_Offsets[currentSpinState*2 + 0] + 1; int_fast8_t prevYKick = O_Offsets[currentSpinState*2 + 1] + 1;

        pieceXPos += prevXKick - targXKick;
        pieceYPos += prevYKick - targYKick;
        currentSpinState = desiredSpinState;
        playfield_update_phantom();
    }
    else
    {
        int8_t* referenceTable = currentPiece == IBlock ? I_Offsets : JLSTZ_Offsets;
        
        for (int i = 0; i < 10; i+=2)
        {
            int_fast8_t prevXKick = referenceTable[currentSpinState*10 + i]; int_fast8_t prevYKick = referenceTable[currentSpinState*10 + i + 1];
            int_fast8_t targXKick = referenceTable[desiredSpinState*10 + i]; int_fast8_t targYKick = referenceTable[desiredSpinState*10 + i + 1];

            bool willItCollide = playfield_check_collision(currentPiece, desiredSpinState, pieceXPos + (prevXKick - targXKick), pieceYPos + (prevYKick - targYKick));
            if (!willItCollide)
            {
                pieceXPos += prevXKick - targXKick;
                pieceYPos += prevYKick - targYKick;
                currentSpinState = desiredSpinState;
                playfield_update_phantom();
                break;
            }
        }
    }
}

// Move tetromino right or left
// right: true to rotate right, false to rotate left
void playfield_move_current_piece(bool right)
{
    bool willItCollide = playfield_check_collision(currentPiece, currentSpinState, pieceXPos + (right ? 1 : -1), pieceYPos);
    if (!willItCollide)
    {
        pieceXPos += right ? 1 : -1;
        playfield_update_phantom();
    }
}

// Logic to handle transferring pieces from currentPiece to playfield
void playfield_finish_drop()
{
    // Set playfield to have new blocks
    uint8_t Coordinates[8] = {0};

    // Retrieve current tetromino block positions
    tetromino_get_positions(pieceXPos, pieceYPos, currentPiece, currentSpinState, Coordinates);

    // Set these blocks to be active in their respective color
    for (int i = 0; i < 8; i+=2)
    {
        playfield_set_state(Coordinates[i], Coordinates[i+1], currentPiece);
    }

    currentPiece = nextPiece;
    currentSpinState = ZeroDeg;
    pieceYPos = 20;
    if (currentPiece == OBlock)
    {
        pieceXPos = 5;
    }
    else
    {
        pieceXPos = 4;
    }
    nextPiece = playfield_get_new_piece();

    checkForLineClear = true;

    playfield_update_phantom();
}

// Soft drop piece
void playfield_soft_drop()
{
    bool willItCollide = playfield_check_collision(currentPiece, currentSpinState, pieceXPos, pieceYPos - 1);
        
    if (!willItCollide)
    {
        pieceYPos--;
    }
    else
    {
        playfield_finish_drop();
    }
}

// Hard drop piece to phantom position
void playfield_hard_drop()
{
    pieceYPos = phantomYPos;
    playfield_finish_drop();
}



// Update phantom tetromino Y Pos
void playfield_update_phantom()
{
    for (int y = pieceYPos; y > 0; y--)
    {
        bool willItCollide = playfield_check_collision(currentPiece, currentSpinState, pieceXPos, y - 1);
        
        if (willItCollide)
        {
            phantomYPos = y;
            break;
        }
    }
}

// Initializes playfield variables
void playfield_init()
{
    // Initialize us at a random state of seen pieces
    seen_pieces = sys_rand8_get() & 0b1111111;
    
    currentPiece = playfield_get_new_piece();
    nextPiece = playfield_get_new_piece();
    playfield_update_phantom();
}

// Update current playfield status
// delta_time: how much time passed since last frame
// elapsed_time: how much time passed since boot
void playfield_tick(int64_t delta_time, int64_t elapsed_time, bool *user_data)
{
    if (elapsed_time == 0)
    {
        return;
    }

    if (!user_data[0])
    {
        if (elapsed_time - stopwatch > Timeout && !checkForLineClear)
        {
            playfield_soft_drop();
            stopwatch = elapsed_time;
        }
    }
    else
    {
        uint32_t interval = (pieceYPos != phantomYPos) ? (Timeout >> 2) : Timeout;
        if (elapsed_time - stopwatch > interval && !checkForLineClear)
        {
            playfield_soft_drop();
            stopwatch = elapsed_time;
        }
    }

    tetromino_draw_phantom(pieceXPos, phantomYPos - 1, currentPiece, currentSpinState);
    tetromino_draw(pieceXPos, pieceYPos - 1, currentPiece, currentSpinState);

    if (checkForLineClear && !finishedCheck)
    {
        // clear indexes to shift
        memset(indexesToShift, 0, sizeof(indexesToShift));
        uint_fast8_t linesCleared = 0;

        for (int i = 0; i < 22; i++)
        {
            bool wholeLineOccupied = true;
            uint32_t PlayfieldRow = Playfield[i];

            for (int j = 0; j < 10; j++)
            {
                uint32_t tileMask = (0b111 << (j * 3));
                
                wholeLineOccupied &= ((PlayfieldRow & tileMask) != 0);
                
                if (!wholeLineOccupied)
                {
                    // Let's skip to next line
                    break;
                }

                if (wholeLineOccupied && j == 9)
                {
                    // WE HAVE A LINE CLEAR!
                    indexesToShift[linesCleared] = i - linesCleared; // <- we have to remember that the line in question will be 1 row down when prev line is cleared
                    linesCleared += 1;
                    stopwatch = elapsed_time;
                }
            }
        }

        checkForLineClear = (linesCleared != 0);

        if (checkForLineClear)
        {
            switch (linesCleared)
            {
                case 1:
                    Score += 100 * Level;
                    break;
                case 2:
                    Score += 400 * Level;
                    break;
                case 3:
                    Score += 900 * Level;
                    break;
                case 4:
                    Score += 1600 * Level;
                    break;
                default:
                    break;
            }

            Score += 50 * Combo * Level;

            TotalLinesCleared += linesCleared * linesCleared;

            while (TotalLinesCleared > Level * 10)
            {
                TotalLinesCleared -= Level * 10;
                Level++;
                if (Level <= 8)
                {
                    Timeout -= 83;
                }
                else if (Level == 9)
                {
                    Timeout -= 33;
                }
                else if (Level == 10)
                {
                    Timeout -= 17;
                }
                else if (Level <= 19)
                {
                    Timeout -= 5;
                }
                else if (Level <= 28)
                {
                    Timeout -= 2;
                }
            }

            Combo++;
        }
        finishedCheck = true;
    }
    else if (checkForLineClear && finishedCheck)
    {
        // Translate occupied lines one row down using memmove!
        // We are only at most clearing 4 lines at a time, so the translation array will only
        // have to look at a depth of 4. Unused spaces are labeled as 0, so when we find a 0,
        // we stop.

        for (int i = 0; i < 4; i++)
        {
            uint_fast8_t index = indexesToShift[i];
            if (index == 0)
            {
                break;
            }
            
            // Overlap-safe version of memcpy!
            memmove(&Playfield[index], &Playfield[index+1], sizeof(uint32_t)*(21-index));
            Playfield[21] = 0;
        }

        checkForLineClear = false;
        finishedCheck = false;
    }
    else if (!checkForLineClear && finishedCheck)
    {
        finishedCheck = false;
    }
}