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

const uint8_t sprite_array[728] = {
    6, 0b111111, 0b100001, 0b101101, 0b101101, 0b100001, 0b111111,
    6, 0b111111, 0b110101, 0b101011, 0b110101, 0b101011, 0b111111,
    6, 0b111111, 0b100001, 0b100001, 0b100001, 0b100001, 0b111111,
    6, 0b111111, 0b101101, 0b110011, 0b110011, 0b101101, 0b111111,
    6, 0b111111, 0b110011, 0b101101, 0b101101, 0b110011, 0b111111,
    6, 0b111111, 0b100011, 0b100101, 0b101001, 0b110001, 0b111111,
    6, 0b111111, 0b110001, 0b101001, 0b100101, 0b100011, 0b111111,
    4, 0b111110, 0b000101, 0b000101, 0b111110, 0b000000, 0b000000,
    4, 0b111111, 0b100101, 0b100101, 0b011010, 0b000000, 0b000000,
    4, 0b011110, 0b100001, 0b100001, 0b010010, 0b000000, 0b000000,
    4, 0b111111, 0b100001, 0b100001, 0b011110, 0b000000, 0b000000,
    4, 0b111111, 0b100101, 0b100101, 0b100001, 0b000000, 0b000000,
    4, 0b111111, 0b000101, 0b000101, 0b000001, 0b000000, 0b000000,
    4, 0b011110, 0b100001, 0b101001, 0b111010, 0b000000, 0b000000,
    4, 0b111111, 0b000100, 0b000100, 0b111111, 0b000000, 0b000000,
    3, 0b100001, 0b111111, 0b100001, 0b000000, 0b000000, 0b000000,
    3, 0b010000, 0b100001, 0b011111, 0b000000, 0b000000, 0b000000,
    4, 0b111111, 0b000100, 0b001010, 0b110001, 0b000000, 0b000000,
    3, 0b111111, 0b100000, 0b100000, 0b000000, 0b000000, 0b000000,
    5, 0b111111, 0b000010, 0b000100, 0b000010, 0b111111, 0b000000,
    4, 0b111111, 0b000110, 0b011000, 0b111111, 0b000000, 0b000000,
    4, 0b011110, 0b100001, 0b100001, 0b011110, 0b000000, 0b000000,
    4, 0b111111, 0b000101, 0b000101, 0b000010, 0b000000, 0b000000,
    5, 0b011110, 0b100001, 0b110001, 0b111110, 0b100000, 0b000000,
    4, 0b111111, 0b000101, 0b000101, 0b111010, 0b000000, 0b000000,
    4, 0b100010, 0b100101, 0b100101, 0b011001, 0b000000, 0b000000,
    5, 0b000001, 0b000001, 0b111111, 0b000001, 0b000001, 0b000000,
    4, 0b011111, 0b100000, 0b100000, 0b011111, 0b000000, 0b000000,
    5, 0b000011, 0b001100, 0b110000, 0b001100, 0b000011, 0b000000,
    5, 0b011111, 0b100000, 0b011110, 0b100000, 0b011111, 0b000000,
    3, 0b110011, 0b001100, 0b110011, 0b000000, 0b000000, 0b000000,
    3, 0b000111, 0b111000, 0b000111, 0b000000, 0b000000, 0b000000,
    4, 0b110001, 0b101001, 0b100101, 0b100011, 0b000000, 0b000000,
    4, 0b011000, 0b100100, 0b100100, 0b111000, 0b000000, 0b000000,
    4, 0b111111, 0b100100, 0b100100, 0b011000, 0b000000, 0b000000,
    3, 0b011000, 0b100100, 0b100100, 0b000000, 0b000000, 0b000000,
    4, 0b011000, 0b100100, 0b100100, 0b111111, 0b000000, 0b000000,
    4, 0b011000, 0b110100, 0b110100, 0b101000, 0b000000, 0b000000,
    3, 0b111110, 0b000101, 0b000001, 0b000000, 0b000000, 0b000000,
    3, 0b101000, 0b110100, 0b111000, 0b000000, 0b000000, 0b000000,
    4, 0b111111, 0b001000, 0b000100, 0b111000, 0b000000, 0b000000,
    1, 0b111010, 0b000000, 0b000000, 0b000000, 0b000000, 0b000000,
    2, 0b100000, 0b011010, 0b000000, 0b000000, 0b000000, 0b000000,
    3, 0b111111, 0b001000, 0b110100, 0b000000, 0b000000, 0b000000,
    2, 0b011111, 0b100000, 0b000000, 0b000000, 0b000000, 0b000000,
    5, 0b111100, 0b000100, 0b011000, 0b000100, 0b111000, 0b000000,
    4, 0b111100, 0b000100, 0b000100, 0b111000, 0b000000, 0b000000,
    4, 0b011000, 0b100100, 0b100100, 0b011000, 0b000000, 0b000000,
    3, 0b111100, 0b010100, 0b001000, 0b000000, 0b000000, 0b000000,
    3, 0b001000, 0b010100, 0b111100, 0b000000, 0b000000, 0b000000,
    3, 0b111100, 0b001000, 0b000100, 0b000000, 0b000000, 0b000000,
    3, 0b101000, 0b101100, 0b010100, 0b000000, 0b000000, 0b000000,
    3, 0b011100, 0b101000, 0b101000, 0b000000, 0b000000, 0b000000,
    4, 0b011100, 0b100000, 0b100000, 0b011100, 0b000000, 0b000000,
    4, 0b001100, 0b010000, 0b100000, 0b111100, 0b000000, 0b000000,
    5, 0b011100, 0b100000, 0b011000, 0b100000, 0b011100, 0b000000,
    3, 0b100100, 0b011000, 0b100100, 0b000000, 0b000000, 0b000000,
    3, 0b101100, 0b110000, 0b011100, 0b000000, 0b000000, 0b000000,
    3, 0b110100, 0b101100, 0b100100, 0b000000, 0b000000, 0b000000,
    4, 0b011110, 0b111001, 0b100111, 0b011110, 0b000000, 0b000000,
    2, 0b000010, 0b111111, 0b000000, 0b000000, 0b000000, 0b000000,
    4, 0b110010, 0b101001, 0b100101, 0b100010, 0b000000, 0b000000,
    4, 0b010010, 0b100001, 0b100101, 0b011010, 0b000000, 0b000000,
    4, 0b001100, 0b001010, 0b111111, 0b001000, 0b000000, 0b000000,
    4, 0b100111, 0b100101, 0b100101, 0b011001, 0b000000, 0b000000,
    4, 0b011110, 0b100101, 0b100101, 0b011000, 0b000000, 0b000000,
    4, 0b000001, 0b110001, 0b001101, 0b000011, 0b000000, 0b000000,
    4, 0b011010, 0b100101, 0b100101, 0b011010, 0b000000, 0b000000,
    4, 0b000110, 0b101001, 0b101001, 0b011110, 0b000000, 0b000000,
    1, 0b101111, 0b000000, 0b000000, 0b000000, 0b000000, 0b000000,
    3, 0b000010, 0b101001, 0b000110, 0b000000, 0b000000, 0b000000,
    6, 0b000110, 0b000110, 0b011110, 0b011110, 0b000110, 0b000110,
    5, 0b000000, 0b011110, 0b011110, 0b011110, 0b011110, 0b000000,
    6, 0b110000, 0b111000, 0b011100, 0b001110, 0b000111, 0b000011,
    6, 0b000110, 0b000110, 0b011110, 0b011110, 0b011000, 0b011000,
    6, 0b011000, 0b011000, 0b011110, 0b011110, 0b000110, 0b000110,
    5, 0b000000, 0b111111, 0b111111, 0b110000, 0b110000, 0b000000,
    5, 0b000000, 0b110000, 0b110000, 0b111111, 0b111111, 0b000000,
    1, 0b110011, 0b000000, 0b000000, 0b000000, 0b000000, 0b000000,
    3, 0b110000, 0b001100, 0b000011, 0b000000, 0b000000, 0b000000,
    3, 0b000011, 0b001100, 0b110000, 0b000000, 0b000000, 0b000000,
    1, 0b100000, 0b000000, 0b000000, 0b000000, 0b000000, 0b000000,
    2, 0b100000, 0b010000, 0b000000, 0b000000, 0b000000, 0b000000,
    5, 0b000100, 0b000010, 0b000001, 0b000010, 0b000100, 0b000000,
    5, 0b110100, 0b011100, 0b001111, 0b011100, 0b110100, 0b000000,
    5, 0b100010, 0b010100, 0b111110, 0b010100, 0b100010, 0b000000,
    6, 0b110100, 0b011100, 0b010111, 0b110100, 0b011100, 0b010111,
    4, 0b100100, 0b010000, 0b001000, 0b100100, 0b000000, 0b000000,
    3, 0b001000, 0b011100, 0b001000, 0b000000, 0b000000, 0b000000,
    3, 0b001000, 0b001000, 0b001000, 0b000000, 0b000000, 0b000000,
    3, 0b010100, 0b001000, 0b010100, 0b000000, 0b000000, 0b000000,
    3, 0b001000, 0b101010, 0b001000, 0b000000, 0b000000, 0b000000,
    5, 0b001000, 0b010010, 0b010000, 0b010010, 0b001000, 0b000000,
    6, 0b001110, 0b011111, 0b111110, 0b111110, 0b011111, 0b001110,
    6, 0b110000, 0b111000, 0b111110, 0b111110, 0b111000, 0b110000,
    6, 0b001100, 0b001100, 0b001100, 0b111111, 0b011110, 0b001100,
    6, 0b001100, 0b011110, 0b111111, 0b001100, 0b001100, 0b001100,
    6, 0b000100, 0b000110, 0b111111, 0b111111, 0b000110, 0b000100,
    6, 0b001000, 0b011000, 0b111111, 0b111111, 0b011000, 0b001000,
    6, 0b011111, 0b000111, 0b001111, 0b011101, 0b111001, 0b110000,
    6, 0b110000, 0b111001, 0b011101, 0b001111, 0b000111, 0b011111,
    6, 0b000011, 0b100111, 0b101110, 0b111100, 0b111000, 0b111110,
    6, 0b111110, 0b111000, 0b111100, 0b101110, 0b100111, 0b000011,
    6, 0b010000, 0b101010, 0b100101, 0b101010, 0b010000, 0b101000
};

///////////////////////////////////////////////////////////
// ^ THE CONTENTS ABOVE WERE PROGRAMATICALLY GENERATED ^ //

// Draws sprite based on sprite id defined in rsc/sprites.bmp at specified coordinate using transposed coordinate system
// sprite_id: Which sprite to draw
// x:  x coordinate
// y:  y coordinate
//
// Returns horizontal resolution of the drawn sprite
uint8_t sprite_draw(uint_fast8_t sprite_id, uint_fast8_t x, uint_fast8_t y) 
{
    if (sprite_id >= SPRITE_NUM)
    {
        return 0;
    }

    const uint8_t *sprite_to_draw = &sprite_array[sprite_id * 7];
    const uint8_t hor_res = sprite_to_draw[0];
    
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
bool sprite_draw_block(uint_fast8_t x, uint_fast8_t y, uint_fast8_t block_type)
{
    if (x >= 10 || y >= 18) { return false; }

    uint_fast8_t x_raw = y * 6 + 2;
    uint_fast8_t y_raw = x * 6 + 2;

    if (block_type < 7)
    {
        sprite_draw(block_type & 7, x_raw, y_raw);
    } 
    else
    {
        gfx_fill(x_raw, y_raw, 6, 6, false);
    }

    return true;
}

// Writes text using sprites at specified coordinate using transposed coordinate system
// x:        x coordinate
// y:        y coordinate
// sentence: *null-terminated* string
void sprite_draw_text(uint_fast8_t x, uint_fast8_t y, char *sentence)
{
    uint_fast8_t index = 0;
    uint_fast8_t caret_distance = 0;
    while(sentence[index] != '\0')
    {
        int char_code = (int)sentence[index];
        if (char_code > 64 && char_code < 91) // Upper case letters
        {
            caret_distance += 1 + sprite_draw(char_code - 58, x, y + caret_distance);
        }
        else if (char_code > 96 && char_code < 123) // Lower case letters
        {
            caret_distance += 1 + sprite_draw(char_code - 64, x, y + caret_distance);
        }
        else if (char_code > 47 && char_code < 58) // 0-9 digits
        {
            caret_distance += 1 + sprite_draw(char_code + 11, x, y + caret_distance);
        }
        else if (char_code == 32) // Space
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

void sprite_draw_number(uint_fast8_t x, uint_fast8_t y, int32_t number, uint_fast8_t base)
{
    int32_t remainder = number;
    uint_fast8_t caret_distance = 0;
    uint32_t digit_count = 0;

    uint8_t char_ids[32] = {59};

    if (base > 62)
    {
        return;
    }

    if (remainder < 0)
    {
        caret_distance += 1 + sprite_draw(89, x, y + caret_distance);
        remainder *= -1;
    } else if (remainder == 0)
    {
        digit_count = 1;
    }

    while (remainder != 0 && digit_count < 32)
    {
        uint_fast8_t digit_value = remainder % base;
    
        char_ids[digit_count++] = ((digit_value + 52) % 62) + 7; // Start with 0-9, then cycle back to capital letters, based on sprite IDs.
        remainder = remainder / base;
    }

    for (int i = digit_count - 1; i >= 0; i--)
    {
        caret_distance += 1 + sprite_draw(char_ids[i], x, y + caret_distance);
    }
}