// This file contains programatically generated content via sprite_factory.py.
// Whatever you write inside there will be overriden when executing the program.

// HOW SPRITES ARE STORED:
// They get transformed from their bitmap representations into an array of uint8_ts, where the first byte is the horizontal
// resolution of the sprite itself, and then subsequent bytes represent in little endian format whether the next pixel is on
// or off.

#include "sprites.h"
#include "graphics.h"

// v THE CONTENTS BELOW WERE PROGRAMATICALLY GENERATED v //
///////////////////////////////////////////////////////////
uint8_t sprite0[7] = { 6, 0b111111, 0b100001, 0b101101, 0b101101, 0b100001, 0b111111};
uint8_t sprite1[7] = { 6, 0b111111, 0b110101, 0b101011, 0b110101, 0b101011, 0b111111};
uint8_t sprite2[7] = { 6, 0b111111, 0b100001, 0b100001, 0b100001, 0b100001, 0b111111};
uint8_t sprite3[7] = { 6, 0b111111, 0b101101, 0b110011, 0b110011, 0b101101, 0b111111};
uint8_t sprite4[7] = { 6, 0b111111, 0b110011, 0b101101, 0b101101, 0b110011, 0b111111};
uint8_t sprite5[7] = { 6, 0b111111, 0b100011, 0b100101, 0b101001, 0b110001, 0b111111};
uint8_t sprite6[7] = { 6, 0b111111, 0b110001, 0b101001, 0b100101, 0b100011, 0b111111};
uint8_t sprite7[5] = { 4, 0b111110, 0b000101, 0b000101, 0b111110};
uint8_t sprite8[5] = { 4, 0b111111, 0b100101, 0b100101, 0b011010};
uint8_t sprite9[5] = { 4, 0b011110, 0b100001, 0b100001, 0b010010};
uint8_t sprite10[5] = { 4, 0b111111, 0b100001, 0b100001, 0b011110};
uint8_t sprite11[5] = { 4, 0b111111, 0b100101, 0b100101, 0b100001};
uint8_t sprite12[5] = { 4, 0b111111, 0b000101, 0b000101, 0b000001};
uint8_t sprite13[5] = { 4, 0b011110, 0b100001, 0b101001, 0b111010};
uint8_t sprite14[5] = { 4, 0b111111, 0b000100, 0b000100, 0b111111};
uint8_t sprite15[4] = { 3, 0b100001, 0b111111, 0b100001};
uint8_t sprite16[4] = { 3, 0b010000, 0b100001, 0b011111};
uint8_t sprite17[5] = { 4, 0b111111, 0b000100, 0b001010, 0b110001};
uint8_t sprite18[4] = { 3, 0b111111, 0b100000, 0b100000};
uint8_t sprite19[6] = { 5, 0b111111, 0b000010, 0b000100, 0b000010, 0b111111};
uint8_t sprite20[5] = { 4, 0b111111, 0b000110, 0b011000, 0b111111};
uint8_t sprite21[5] = { 4, 0b011110, 0b100001, 0b100001, 0b011110};
uint8_t sprite22[5] = { 4, 0b111111, 0b000101, 0b000101, 0b000010};
uint8_t sprite23[6] = { 5, 0b011110, 0b100001, 0b110001, 0b111110, 0b100000};
uint8_t sprite24[5] = { 4, 0b111111, 0b000101, 0b000101, 0b111010};
uint8_t sprite25[5] = { 4, 0b100010, 0b100101, 0b100101, 0b011001};
uint8_t sprite26[6] = { 5, 0b000001, 0b000001, 0b111111, 0b000001, 0b000001};
uint8_t sprite27[5] = { 4, 0b011111, 0b100000, 0b100000, 0b011111};
uint8_t sprite28[6] = { 5, 0b000011, 0b001100, 0b110000, 0b001100, 0b000011};
uint8_t sprite29[6] = { 5, 0b011111, 0b100000, 0b011110, 0b100000, 0b011111};
uint8_t sprite30[4] = { 3, 0b110011, 0b001100, 0b110011};
uint8_t sprite31[4] = { 3, 0b000111, 0b111000, 0b000111};
uint8_t sprite32[5] = { 4, 0b110001, 0b101001, 0b100101, 0b100011};
uint8_t sprite33[5] = { 4, 0b011000, 0b100100, 0b100100, 0b111000};
uint8_t sprite34[5] = { 4, 0b111111, 0b100100, 0b100100, 0b011000};
uint8_t sprite35[4] = { 3, 0b011000, 0b100100, 0b100100};
uint8_t sprite36[5] = { 4, 0b011000, 0b100100, 0b100100, 0b111111};
uint8_t sprite37[5] = { 4, 0b011000, 0b110100, 0b110100, 0b101000};
uint8_t sprite38[4] = { 3, 0b111110, 0b000101, 0b000001};
uint8_t sprite39[4] = { 3, 0b101000, 0b110100, 0b111000};
uint8_t sprite40[5] = { 4, 0b111111, 0b001000, 0b000100, 0b111000};
uint8_t sprite41[2] = { 1, 0b111010};
uint8_t sprite42[3] = { 2, 0b100000, 0b011010};
uint8_t sprite43[4] = { 3, 0b111111, 0b001000, 0b110100};
uint8_t sprite44[3] = { 2, 0b011111, 0b100000};
uint8_t sprite45[6] = { 5, 0b111100, 0b000100, 0b011000, 0b000100, 0b111000};
uint8_t sprite46[5] = { 4, 0b111100, 0b000100, 0b000100, 0b111000};
uint8_t sprite47[5] = { 4, 0b011000, 0b100100, 0b100100, 0b011000};
uint8_t sprite48[4] = { 3, 0b111100, 0b010100, 0b001000};
uint8_t sprite49[4] = { 3, 0b001000, 0b010100, 0b111100};
uint8_t sprite50[4] = { 3, 0b111100, 0b001000, 0b000100};
uint8_t sprite51[4] = { 3, 0b101000, 0b101100, 0b010100};
uint8_t sprite52[4] = { 3, 0b011100, 0b101000, 0b101000};
uint8_t sprite53[5] = { 4, 0b011100, 0b100000, 0b100000, 0b011100};
uint8_t sprite54[5] = { 4, 0b001100, 0b010000, 0b100000, 0b111100};
uint8_t sprite55[6] = { 5, 0b011100, 0b100000, 0b011000, 0b100000, 0b011100};
uint8_t sprite56[4] = { 3, 0b100100, 0b011000, 0b100100};
uint8_t sprite57[4] = { 3, 0b101100, 0b110000, 0b011100};
uint8_t sprite58[4] = { 3, 0b110100, 0b101100, 0b100100};
uint8_t sprite59[5] = { 4, 0b011110, 0b111001, 0b100111, 0b011110};
uint8_t sprite60[3] = { 2, 0b000010, 0b111111};
uint8_t sprite61[5] = { 4, 0b110010, 0b101001, 0b100101, 0b100010};
uint8_t sprite62[5] = { 4, 0b010010, 0b100001, 0b100101, 0b011010};
uint8_t sprite63[5] = { 4, 0b001100, 0b001010, 0b111111, 0b001000};
uint8_t sprite64[5] = { 4, 0b100111, 0b100101, 0b100101, 0b011001};
uint8_t sprite65[5] = { 4, 0b011110, 0b100101, 0b100101, 0b011000};
uint8_t sprite66[5] = { 4, 0b000001, 0b110001, 0b001101, 0b000011};
uint8_t sprite67[5] = { 4, 0b011010, 0b100101, 0b100101, 0b011010};
uint8_t sprite68[5] = { 4, 0b000110, 0b101001, 0b101001, 0b011110};
uint8_t sprite69[2] = { 1, 0b101111};
uint8_t sprite70[4] = { 3, 0b000010, 0b101001, 0b000110};
uint8_t sprite71[7] = { 6, 0b001100, 0b001100, 0b111100, 0b111100, 0b001100, 0b001100};
uint8_t sprite72[5] = { 4, 0b111111, 0b111111, 0b110000, 0b110000};
uint8_t sprite73[5] = { 4, 0b110000, 0b110000, 0b111111, 0b111111};
uint8_t sprite74[5] = { 4, 0b111100, 0b111100, 0b111100, 0b111100};
uint8_t sprite75[7] = { 6, 0b110000, 0b110000, 0b111100, 0b111100, 0b001100, 0b001100};
uint8_t sprite76[7] = { 6, 0b001100, 0b001100, 0b111100, 0b111100, 0b110000, 0b110000};
uint8_t sprite77[7] = { 6, 0b110000, 0b111000, 0b011100, 0b001110, 0b000111, 0b000011};
uint8_t sprite78[2] = { 1, 0b110011};
uint8_t sprite79[4] = { 3, 0b110000, 0b001100, 0b000011};
uint8_t sprite80[4] = { 3, 0b000011, 0b001100, 0b110000};
uint8_t sprite81[2] = { 1, 0b100000};
uint8_t sprite82[3] = { 2, 0b100000, 0b010000};
uint8_t sprite83[6] = { 5, 0b000100, 0b000010, 0b000001, 0b000010, 0b000100};
uint8_t sprite84[6] = { 5, 0b110100, 0b011100, 0b001111, 0b011100, 0b110100};
uint8_t sprite85[6] = { 5, 0b100010, 0b010100, 0b111110, 0b010100, 0b100010};
uint8_t sprite86[7] = { 6, 0b110100, 0b011100, 0b010111, 0b110100, 0b011100, 0b010111};
uint8_t sprite87[5] = { 4, 0b100100, 0b010000, 0b001000, 0b100100};
uint8_t sprite88[4] = { 3, 0b001000, 0b011100, 0b001000};
uint8_t sprite89[4] = { 3, 0b001000, 0b001000, 0b001000};
uint8_t sprite90[4] = { 3, 0b010100, 0b001000, 0b010100};
uint8_t sprite91[4] = { 3, 0b001000, 0b101010, 0b001000};
uint8_t sprite92[6] = { 5, 0b001000, 0b010010, 0b010000, 0b010010, 0b001000};
uint8_t sprite93[7] = { 6, 0b001110, 0b011111, 0b111110, 0b111110, 0b011111, 0b001110};
uint8_t sprite94[7] = { 6, 0b110000, 0b111000, 0b111110, 0b111110, 0b111000, 0b110000};
uint8_t sprite95[7] = { 6, 0b001100, 0b001100, 0b001100, 0b111111, 0b011110, 0b001100};
uint8_t sprite96[7] = { 6, 0b001100, 0b011110, 0b111111, 0b001100, 0b001100, 0b001100};
uint8_t sprite97[7] = { 6, 0b000100, 0b000110, 0b111111, 0b111111, 0b000110, 0b000100};
uint8_t sprite98[7] = { 6, 0b001000, 0b011000, 0b111111, 0b111111, 0b011000, 0b001000};
uint8_t sprite99[7] = { 6, 0b011111, 0b000111, 0b001111, 0b011101, 0b111001, 0b110000};
uint8_t sprite100[7] = { 6, 0b110000, 0b111001, 0b011101, 0b001111, 0b000111, 0b011111};
uint8_t sprite101[7] = { 6, 0b000011, 0b100111, 0b101110, 0b111100, 0b111000, 0b111110};
uint8_t sprite102[7] = { 6, 0b111110, 0b111000, 0b111100, 0b101110, 0b100111, 0b000011};
uint8_t sprite103[7] = { 6, 0b010000, 0b101010, 0b100101, 0b101010, 0b010000, 0b101000};

uint8_t *spriteArray[] = {
    sprite0,
    sprite1,
    sprite2,
    sprite3,
    sprite4,
    sprite5,
    sprite6,
    sprite7,
    sprite8,
    sprite9,
    sprite10,
    sprite11,
    sprite12,
    sprite13,
    sprite14,
    sprite15,
    sprite16,
    sprite17,
    sprite18,
    sprite19,
    sprite20,
    sprite21,
    sprite22,
    sprite23,
    sprite24,
    sprite25,
    sprite26,
    sprite27,
    sprite28,
    sprite29,
    sprite30,
    sprite31,
    sprite32,
    sprite33,
    sprite34,
    sprite35,
    sprite36,
    sprite37,
    sprite38,
    sprite39,
    sprite40,
    sprite41,
    sprite42,
    sprite43,
    sprite44,
    sprite45,
    sprite46,
    sprite47,
    sprite48,
    sprite49,
    sprite50,
    sprite51,
    sprite52,
    sprite53,
    sprite54,
    sprite55,
    sprite56,
    sprite57,
    sprite58,
    sprite59,
    sprite60,
    sprite61,
    sprite62,
    sprite63,
    sprite64,
    sprite65,
    sprite66,
    sprite67,
    sprite68,
    sprite69,
    sprite70,
    sprite71,
    sprite72,
    sprite73,
    sprite74,
    sprite75,
    sprite76,
    sprite77,
    sprite78,
    sprite79,
    sprite80,
    sprite81,
    sprite82,
    sprite83,
    sprite84,
    sprite85,
    sprite86,
    sprite87,
    sprite88,
    sprite89,
    sprite90,
    sprite91,
    sprite92,
    sprite93,
    sprite94,
    sprite95,
    sprite96,
    sprite97,
    sprite98,
    sprite99,
    sprite100,
    sprite101,
    sprite102,
    sprite103
};
///////////////////////////////////////////////////////////
// ^ THE CONTENTS ABOVE WERE PROGRAMATICALLY GENERATED ^ //

// Draws sprite based on sprite id defined in rsc/sprites.bmp at specified coordinate using transposed coordinate system
// sprite_id: Which sprite to draw
// x:  x coordinate
// y:  y coordinate
//
// Returns horizontal resolution of the drawn sprite
uint8_t sprite_draw(uint8_t sprite_id, uint8_t x, uint8_t y) 
{
    if (sprite_id >= SPRITE_NUM)
    {
        return 0;
    }

    uint8_t *sprite_to_draw = spriteArray[sprite_id];
    uint8_t hor_res = sprite_to_draw[0];
    
    for (int i = 0; i < hor_res; i++)
    {
        uint8_t column = sprite_to_draw[i + 1];
        gfx_px(x + 5, y + i, column & 1);
        gfx_px(x + 4, y + i, column & 2);
        gfx_px(x + 3, y + i, column & 4);
        gfx_px(x + 2, y + i, column & 8);
        gfx_px(x + 1, y + i, column & 16);
        gfx_px(x + 0, y + i, column & 32);
    }
    
    return hor_res;
}

// Draws 6px-wide block using tetrimino coordinate system
// x:  x coordinate
// y:  y coordinate
// block_type: which block to draw based on block enum ids
void sprite_draw_block(uint8_t x, uint8_t y, uint8_t block_type)
{
    // TODO: Change these to a table format to save on time??
    if (x >= 10 || y >= 18) { return; }

    uint8_t x_raw = y * 6 + 2;
    uint8_t y_raw = x * 6 + 2;

    sprite_draw(block_type & 7, x_raw, y_raw);
}

// Writes text using sprites at specified coordinate using transposed coordinate system
// x:        x coordinate
// y:        y coordinate
// sentence: *null-terminated* string
void sprite_draw_text(uint8_t x, uint8_t y, char *sentence)
{
    uint8_t index = 0;
    uint8_t caret_distance = 0;
    while(sentence[index] != '\0')
    {
        int char_code = (int)sentence[index];
        if (char_code > 64 && char_code < 91)
        {
            caret_distance += 1 + sprite_draw(char_code - 58, x, y + caret_distance);
        }
        else if (char_code > 96 && char_code < 123)
        {
            caret_distance += 1 + sprite_draw(char_code - 64, x, y + caret_distance);
        }
        else if (char_code > 47 && char_code < 58)
        {
            caret_distance += 1 + sprite_draw(char_code + 11, x, y + caret_distance);
        }
        else if (char_code == 32)
        {
            caret_distance += 3;
        }
        else
        {
            int sprite_code = 0;
            switch (sentence[index])
            {
                case '!':
                    sprite_code = 69;
                    break;
                    
                case ':':
                    sprite_code = 78;
                    break;
                    
                case '/':
                    sprite_code = 79;
                    break;
                    
                case '\\':
                    sprite_code = 80;
                    break;
                    
                case '.':
                    sprite_code = 81;
                    break;
                    
                case ',':
                    sprite_code = 82;
                    break;
                    
                case '^':
                    sprite_code = 83;
                    break;
                    
                case '*':
                    sprite_code = 85;
                    break;
                    
                case '#':
                    sprite_code = 86;
                    break;
                    
                case '%':
                    sprite_code = 87;
                    break;
                    
                case '+':
                    sprite_code = 88;
                    break;
                    
                case '-':
                    sprite_code = 89;
                    break;

                case '&':
                    sprite_code = 103;
                    break;
                    
                case '?':
                default:
                    sprite_code = 70;
                    break;
                }

                caret_distance += 1 + sprite_draw(sprite_code, x, y + caret_distance);
            }
            
            index++;
        }
    }