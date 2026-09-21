#include "playfield.h"
#include <stdio.h>

uint8_t OBLockPlayfield   [28] = {0};
uint8_t TBLockPlayfield   [28] = {0};
uint8_t IBLockPlayfield   [28] = {0};
uint8_t JBLockPlayfield   [28] = {0};
uint8_t LBLockPlayfield   [28] = {0};
uint8_t SBLockPlayfield   [28] = {0};
uint8_t ZBLockPlayfield   [28] = {0};
uint8_t OccupiedPlayfield [28] = {0};

// Set state of block on bitfield based on tetrimino-coordinates
// x:          x coordinate
// y:          y coordinate
// block_type: which block to set state of
// state:      whether to turn block on or off
void playfield_set_state(uint8_t x, uint8_t y, Tetromino block_type, bool state)
{
    if (x >= 10 || y >= 22) { return; }

    uint_fast8_t arrayCoord = (x + y*10);
    uint_fast8_t arrayRow = 1 << (arrayCoord & 7);
    uint_fast8_t arrayColumn = arrayCoord >> 3;

    uint8_t *columnToChange = &(OBLockPlayfield[0]);

    switch (block_type)
    {
        case OBlock:
            columnToChange = &(OBLockPlayfield[arrayColumn]);
            break;
            
        case IBlock:
            columnToChange = &(IBLockPlayfield[arrayColumn]);
            break;
            
        case JBlock:
            columnToChange = &(JBLockPlayfield[arrayColumn]);
            break;
            
        case LBlock:
            columnToChange = &(LBLockPlayfield[arrayColumn]);
            break;
            
        case SBlock:
            columnToChange = &(SBLockPlayfield[arrayColumn]);
            break;
            
        case ZBlock:
            columnToChange = &(ZBLockPlayfield[arrayColumn]);
            break;
            
        case TBlock:
            columnToChange = &(TBLockPlayfield[arrayColumn]);
            break;
                    
        default:
            return;
    }

    if (state) {
        *columnToChange |= arrayRow;
    } else {
        *columnToChange &= ~arrayRow;
    }
}

// Updates status of occupied playfield
void playfield_update_occupied()
{
    for (int i = 0; i < 7; i++)
    {
        uint8_t *fieldToOr = OBLockPlayfield;

        switch (i)
        {
            case OBlock:
                fieldToOr = OBLockPlayfield;
                break;
                
            case IBlock:
                fieldToOr = IBLockPlayfield;
                break;
                
            case JBlock:
                fieldToOr = JBLockPlayfield;
                break;
                
            case LBlock:
                fieldToOr = LBLockPlayfield;
                break;
                
            case SBlock:
                fieldToOr = SBLockPlayfield;
                break;
                
            case ZBlock:
                fieldToOr = ZBLockPlayfield;
                break;
                
            case TBlock:
                fieldToOr = TBLockPlayfield;
                break;
                        
            default:
                return;
        }

        for (int j = 0; j < 28; j+=4)
        {
            OccupiedPlayfield[j+0] |= fieldToOr[j+0];
            OccupiedPlayfield[j+1] |= fieldToOr[j+1];
            OccupiedPlayfield[j+2] |= fieldToOr[j+2];
            OccupiedPlayfield[j+3] |= fieldToOr[j+3];
        }
    }
}

// Renders active playfield
void playfield_render()
{
    // Erase old playfield
    gfx_fill(1, 1, 110, 62, false);
    gfx_playfield();

    // Draw individual playfields
    for (uint_fast8_t i = 0; i < 8; i++)
    {
        uint8_t *fieldToDraw = OccupiedPlayfield;
        uint_fast8_t blockStyle = 7;

        switch (i)
        {
            case OBlock:
                fieldToDraw = OBLockPlayfield;
                blockStyle = OBlock;
                break;
                
            case IBlock:
                fieldToDraw = IBLockPlayfield;
                blockStyle = IBlock;
                break;
                
            case JBlock:
                fieldToDraw = JBLockPlayfield;
                blockStyle = JBlock;
                break;
                
            case LBlock:
                fieldToDraw = LBLockPlayfield;
                blockStyle = LBlock;
                break;
                
            case SBlock:
                fieldToDraw = SBLockPlayfield;
                blockStyle = SBlock;
                break;
                
            case ZBlock:
                fieldToDraw = ZBLockPlayfield;
                blockStyle = ZBlock;
                break;
                
            case TBlock:
                fieldToDraw = TBLockPlayfield;
                blockStyle = TBlock;
                break;
                        
            default:
                break;
        }

        // For cleanup, we only want to draw the OccupiedPlayfield (i.e. clear any unused gridspaces)
        // if Occupied is 0.
        int not_cleaning = (i != 7);

        for (uint_fast8_t j = 0; j < 23; j++) // Last few rows are out of view, so we don't need to go through all of them!
        {
            for (uint_fast8_t k = 0; k < 8; k++)
            {
                uint_fast8_t coord = 8 * j + k;
                uint_fast8_t x = coord % 10;
                uint_fast8_t y = coord / 10; 
                uint_fast8_t k_bit = (1 << k);

                if (y >= 18) // Only for four bits but we can avoid going inside other functions for this same if statement!
                {
                    break;
                }

                if ((fieldToDraw[j] & k_bit) == k_bit * not_cleaning)
                {
                    sprite_draw_block(x, y, blockStyle);
                }
            }
        }
    }
}