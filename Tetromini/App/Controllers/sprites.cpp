#include "sprites.hpp"

uint8_t SpritesController::drawSprite(uint_fast8_t sprite_id, uint_fast8_t x, uint_fast8_t y)
{
    if (sprite_id >= SPRITE_NUM)
    {
        return 0;
    }

    const uint8_t *sprite_to_draw = &kSpriteArray[sprite_id * 7];
    const uint8_t hor_res = sprite_to_draw[0];
    
    for (int i = 0; i < hor_res; i++)
    {
        uint8_t column = sprite_to_draw[i + 1];
        _graphics_controller->setPixel(x + 5, y + i, column & 1);
        _graphics_controller->setPixel(x + 4, y + i, column & 2);
        _graphics_controller->setPixel(x + 3, y + i, column & 4);
        _graphics_controller->setPixel(x + 2, y + i, column & 8);
        _graphics_controller->setPixel(x + 1, y + i, column & 16);
        _graphics_controller->setPixel(x + 0, y + i, column & 32);
    }
    
    return hor_res;
}

bool SpritesController::drawBlock(uint_fast8_t x, uint_fast8_t y, uint_fast8_t block_type)
{
    if (x >= 10 || y >= 18 || block_type > 7) { return false; }

    uint_fast8_t x_raw = y * 6 + 2;
    uint_fast8_t y_raw = x * 6 + 2;

    if (block_type > 0)
    {
        drawSprite(block_type - 1, x_raw, y_raw);
    } 
    else
    {
        _graphics_controller->filledRect(x_raw, y_raw, 6, 6, false);
    }

    return true;
}

uint_fast8_t SpritesController::drawText(uint_fast8_t x, uint_fast8_t y, const char *sentence)
{
    uint_fast8_t index = 0;
    uint_fast8_t caret_distance = 0;
    while(sentence[index] != '\0')
    {
        int char_code = (int)sentence[index];
        if (char_code > 64 && char_code < 91) // Upper case letters
        {
            caret_distance += 1 + drawSprite(char_code - 58, x, y + caret_distance);
        }
        else if (char_code > 96 && char_code < 123) // Lower case letters
        {
            caret_distance += 1 + drawSprite(char_code - 64, x, y + caret_distance);
        }
        else if (char_code > 47 && char_code < 58) // 0-9 digits
        {
            caret_distance += 1 + drawSprite(char_code + 11, x, y + caret_distance);
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

                case '(':
                    sprite_code = 105;
                    break;

                case ')':
                    sprite_code = 106;
                    break;
                    
                case '?':
                default:
                    sprite_code = 70;
                    break;
            }

            caret_distance += 1 + drawSprite(sprite_code, x, y + caret_distance);
        }
            
        index++;
    }

    return y + caret_distance;
}

uint_fast8_t SpritesController::drawNumber(uint_fast8_t x, uint_fast8_t y, int32_t number, uint_fast8_t base)
{
    int32_t remainder = number;
    uint_fast8_t caret_distance = 0;
    uint32_t digit_count = 0;

    uint8_t char_ids[32] = {59};

    if (base > 62 || base < 2)
    {
        return 0;
    }

    if (remainder < 0)
    {
        caret_distance += 1 + drawSprite(89, x, y + caret_distance);
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
        caret_distance += 1 + drawSprite(char_ids[i], x, y + caret_distance);
    }

    return y + caret_distance;
}