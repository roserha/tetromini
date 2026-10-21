// This file contains programatically generated content via sprite_factory.py.
// Whatever you write inside there will be overriden when executing the program.
#pragma once

// v THE CONTENTS BELOW WERE PROGRAMATICALLY GENERATED v //
///////////////////////////////////////////////////////////

#define SPRITE_NUM 107
// Storage taken by sprites:                749B

///////////////////////////////////////////////////////////
// ^ THE CONTENTS ABOVE WERE PROGRAMATICALLY GENERATED ^ //


// Previously, I did multiple sprite arrays with pointers stored for each of them.
// That was bad, because although the sprite data only took up 533B, the pointer
// array took up a massive 3.328KB.
// For 104 sprites, dynamic sized arrays are 533B, and static-sized array is 728B.
// So yeah, I'll do my own O(1) lookup based on array size instead of pointer actions.

#include <cstdint>
#include <array>
#include "graphics.hpp"

/// @brief The SpritesController class is responsible for storing the sprites that get drawn to the screen.
class SpritesController {
    public:
        /**
         * @brief Initializes the SpritesController with the associated graphics controller
         */
        SpritesController(GraphicsController* graphics_controller)
            : _graphics_controller(graphics_controller) {}

        /**
         * @brief Draw specific sprite
         * 
         * Draws sprite based on sprite id defined in rsc/sprites.bmp 
         * at specified coordinate using transposed coordinate system
         * 
         * @param sprite_id Which sprite to draw (check class for maximum ID)
         * @param x x coordinate [0,128)
         * @param y y coordinate [0,64)
         * @return uint8_t The total length of the drawn sprite [1,6] if sprite_id is valid, 0 otherwise
         */
        uint8_t drawSprite(uint_fast8_t sprite_id, uint_fast8_t x, uint_fast8_t y);

        /**
         * @brief Draw tetromino block
         * 
         * Draws 6px-wide block using tetrimino coordinate system
         * 
         * @param x x coordinate [0,10]
         * @param y y coordinate [0,18]
         * @param block_type which block to draw based on block enum ids [0,7] (7 means empty block)
         * @return bool `true` if successfully drawn, `false` if block is OOB
         */
        bool drawBlock(uint_fast8_t x, uint_fast8_t y, uint_fast8_t block_type);

        /**
         * @brief Write text
         * 
         * Writes text using sprites at specified coordinate using
         * transposed coordinate system
         * 
         * @param x x coordinate [0,128)
         * @param y y coordinate [0,64)
         * @param sentence *null-terminated* string
         * @return uint8_t y position of the caret after writing text, allows for nested texts/numbers
         */
        uint_fast8_t drawText(uint_fast8_t x, uint_fast8_t y, const char *sentence);

        /**
         * @brief Write number
         * 
         * Writes number using sprites at specified coordinate using
         * transposed coordinate system
         * 
         * @param x x coordinate [0,128)
         * @param y y coordinate [0,64)
         * @param number number to print
         * @param base base to print the number at [2,62]
         * @return uint8_t y position of the caret after writing number, allows for nested texts/numbers
         */
        uint_fast8_t drawNumber(uint_fast8_t x, uint_fast8_t y, int32_t number, uint_fast8_t base);
    private:

        GraphicsController *_graphics_controller;

        // v THE CONTENTS BELOW WERE PROGRAMATICALLY GENERATED v //
        ///////////////////////////////////////////////////////////

        static constexpr std::array<uint8_t, 749> kSpriteArray = {
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
            6, 0b010000, 0b101010, 0b100101, 0b101010, 0b010000, 0b101000,
            6, 0b101010, 0b000001, 0b100000, 0b000001, 0b100000, 0b010101,
            2, 0b011110, 0b100001, 0b000000, 0b000000, 0b000000, 0b000000,
            2, 0b100001, 0b011110, 0b000000, 0b000000, 0b000000, 0b000000
        };

        ///////////////////////////////////////////////////////////
        // ^ THE CONTENTS ABOVE WERE PROGRAMATICALLY GENERATED ^ //
};