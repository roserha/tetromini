#include "playfield.h"
#include <stdio.h>
#include <zephyr/random/random.h>

// We will keep track of the active playfield by borrowing bitboards from chess!!

// Given the playfield is 10x22, with only the first 18 rows visible, we need 220 bits for that. 
// With seven pieces, alongside an ORred bitfield of filled spots, we have 220B of data to store for playfields. Not too shabby!!
// Thing is, 220 doesn't divide evenly into 8, 16 or 32. But if we store 224B of data, we can represent the entire playfield using
// uint32_ts, which can optimize processor cycles a smidge!

// Given each playfield is 220 bits long, the math for x,y,b to array is x + 10*y (+ 224*b)!

// Each playfield is 7 bytes across from each other, so you can either pick (+ 224*b) or just add 7 * b later

uint32_t Playfield [56] = {0};
uint32_t Score = 0;
uint_fast8_t Level = 1;
uint_fast8_t seen_pieces = 0;
Tetromino nextPiece = TBlock;
Tetromino currentPiece = TBlock;
Spin currentSpinState = ZeroDeg;
uint_fast8_t pieceXPos = 7;
uint_fast8_t pieceYPos = 20;
int64_t stopwatch = 0;

// Set state of block on bitfield based on tetrimino-coordinates
// x:          x coordinate
// y:          y coordinate
// block_type: which block to set state of
// state:      whether to turn block on or off
void playfield_set_state(uint8_t x, uint8_t y, Tetromino block_type, bool state)
{
    if (x >= 10 || y >= 22) { return; }

    uint32_t arrayCoord = (x + y*10);
    uint32_t arrayRow = 1 << (arrayCoord & 31); // % 32
    size_t arrayColumn = arrayCoord >> 5;       // / 32

    uint32_t *columnToChange = &(Playfield[arrayColumn + block_type * 7]);

    if (state) {
        *columnToChange |= arrayRow;
    } else {
        *columnToChange &= ~arrayRow;
    }
}

// Updates status of occupied playfield
void playfield_update_occupied()
{
    for (size_t i = 0; i < 49; i+=7)
    {
        // Loop unrolling!
        Playfield[49] |= Playfield[i  ];
        Playfield[50] |= Playfield[i+1];
        Playfield[51] |= Playfield[i+2];
        Playfield[52] |= Playfield[i+3];
        Playfield[53] |= Playfield[i+4];
        Playfield[54] |= Playfield[i+5];
        Playfield[55] |= Playfield[i+6];
    }
}

// Renders active playfield
void playfield_render()
{
    // Draw playfield bounds
    gfx_playfield();

    // Draw individual playfields
    for (size_t i = 0; i < 56; i+=7)
    {
        uint_fast8_t blockStyle = i / 7;

        // For cleanup, we only want to draw the OccupiedPlayfield (i.e. clear any unused gridspaces)
        // if Occupied is 0.
        int not_cleaning = (i < 49);

        for (size_t j = 0; j < 7; j++) // Last few rows are out of view, so we don't need to go through all of them!
        {
            uint32_t Column = Playfield[j + i];
            for (uint_fast8_t k = 0; k < 32; k++)
            {
                uint_fast8_t coord = 32 * j + k;
                uint_fast8_t x = coord % 10;
                uint_fast8_t y = coord / 10; 
                uint32_t k_bit = (1U << k);

                if (y >= 18) // Only for four bits but we can avoid going inside other functions for this same if statement!
                {
                    break;
                }

                if ((Column & k_bit) == (k_bit * not_cleaning))
                {
                    sprite_draw_block(x, y, blockStyle);
                }
            }
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
    sprite_draw(71 + ((int)nextPiece), 113, 57);
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
                    piece = (Tetromino)i;
                    seen_pieces |= pieceCheckIndex;
                    break;
                }
            }
        }
    }

    return piece;
}

// Initializes playfield variables
void playfield_init()
{
    // Initialize us at a random state of seen pieces
    seen_pieces = sys_rand8_get() & 0b1111111;
    
    currentPiece = playfield_get_new_piece();
    nextPiece = playfield_get_new_piece();
    
}

// Check for collision of a hypothetical tetromino
// Returns true if collision happens
bool playfield_check_collision(Tetromino block_type, Spin spin_state, uint_fast8_t x, uint_fast8_t y)
{
    uint8_t Coordinates[8] = {0};
    
    tetromino_get_positions(x, y, block_type, spin_state, Coordinates);

    // Simpler collision: colliding with walls (horizontally or bottom)
    for (int i = 0; i < 7; i ++)
    {
        if ((i & 1) == 0) // Checking x
        {
            if (Coordinates[i] > 10)
            {
                return true;
            }
        }
        else // Checking y
        {
            if (Coordinates[i] > 25)
            {
                return true;
            }
        }
    }

    // Harder collision: colliding with existing playfields
    // Transform x and y array into array coordinates

    uint32_t block_array_coords [4] = { (Coordinates[0] + Coordinates[1]*10),
                                        (Coordinates[2] + Coordinates[3]*10),
                                        (Coordinates[4] + Coordinates[5]*10),
                                        (Coordinates[6] + Coordinates[7]*10)};

    for (int i = 0; i < 4; i++)
    {
        // Check if coordinate we're looking at is set to occupied

        uint32_t arrayRow = 1 << (block_array_coords[i] & 31); // % 32
        size_t arrayColumn = block_array_coords[i] >> 5;       // / 32

        uint32_t columnToCheck = Playfield[arrayColumn + 49];

        if ((columnToCheck & arrayRow) != 0)
        {
            return true;
        }
    }

    // No collisions here!
    return false;
}

// Update current playfield status
void playfield_tick(int64_t delta_time, int64_t elapsed_time)
{
    if (elapsed_time == 0)
    {
        return;
    }

    Score = elapsed_time;
    Level = delta_time;

    if (elapsed_time - stopwatch > 300)
    {
        bool willItCollide = playfield_check_collision(currentPiece, currentSpinState, pieceXPos, pieceYPos - 1);
        
        if (!willItCollide)
        {
            pieceYPos--;
            stopwatch = elapsed_time;
        }

        else
        {
            // Set playfield to have new blocks
            uint8_t Coordinates[8] = {0};
    
            tetromino_get_positions(pieceXPos, pieceYPos, currentPiece, currentSpinState, Coordinates);

            for (int i = 0; i < 8; i+=2)
            {
                playfield_set_state(Coordinates[i], Coordinates[i+1], currentPiece, true);
            }

            playfield_update_occupied();

            pieceXPos = 7;
            pieceYPos = 20;
            currentPiece = nextPiece;
            nextPiece = playfield_get_new_piece();
        }
    }

    tetromino_draw(pieceXPos, pieceYPos, currentPiece, currentSpinState);
}