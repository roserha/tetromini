#include "playfield.h"
#include <stdio.h>

// We will keep track of the active playfield by borrowing bitboards from chess!!

// Given the playfield is 10x22, with only the first 18 rows visible, we need 220 bits for that. 
// With seven pieces, alongside an ORred bitfield of filled spots, we have 220B of data to store for playfields. Not too shabby!!
// Thing is, 220 doesn't divide evenly into 8, 16 or 32. But if we store 224B of data, we can represent the entire playfield using
// uint32_ts, which can optimize processor cycles a smidge!

// Given each playfield is 220 bits long, the math for x,y,b to array is x + 10*y (+ 224*b)!

// Each playfield is 7 bytes across from each other, so you can either pick (+ 224*b) or just add 7 * b later

uint32_t Playfield [56] = {0};

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
    // Erase old playfield
    gfx_fill(1, 1, 110, 62, false);
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