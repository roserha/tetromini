#pragma once
#include "sprites.hpp"
#include <array>

// Which tetromino is to be drawn
typedef enum {
    TBlock = 1,
    OBlock = 2,
    IBlock = 3,
    ZBlock = 4,
    SBlock = 5,
    LBlock = 6,
    JBlock = 7
} Tetromino_t;

// How the tetromino is spun
// Defined per bit to make spinning math easier
// based on simplified direction vector * rotation matrix math
// 1s bit means flip x and ys
// 2s bit means negate direction
typedef enum{
    ZeroDeg = 0b00,     
    TwoSeventyDeg = 0b01,
    OneEightyDeg = 0b10,
    NinetyDeg = 0b11
} Spin_t;

class TetrominoController {
    public:
        // Initializer
        TetrominoController(SpritesController* sprites_controller)
            : _sprites_controller(sprites_controller) {}

        /**
         * @brief Compute tetromino block positions
         * 
         * Computes the global x and y positions of all four blocks 
         * of a tetromino based on its type, spin and center position
         * 
         * @param x x coordinate [0,10]
         * @param y y coordinate [0,18]
         * @param block_type what tetromino this is based on `Tetromino_t`
         * @param spin_state what spin state tetromino is based on `Spin_t`
         * @return std::array<uint8_t, 8> P8-byte long array that stores x and y positions of all four blocks
         */
        std::array<uint8_t, 8> getTetrominoPositions(uint_fast8_t x, uint_fast8_t y, Tetromino_t block_type, Spin_t spin_state);

        /**
         * @brief Draw tetromino
         * 
         * Requests sprites to be drawn for a tetromino with the central block 
         * at (`x`,`y`) using tetromino coordinate system and computed block positions
         * 
         * @param x x coordinate [0,10]
         * @param y y coordinate [0,18]
         * @param block_type what tetromino this is based on `Tetromino_t`
         * @param spin_state what spin state tetromino is based on `Spin_t`
         * @return bool `false` if any of the blocks were OOB, `true` otherwise
         */
        bool drawTetromino(uint_fast8_t x, uint_fast8_t y, Tetromino_t block_type, Spin_t spin_state);

        /**
         * @brief Draw phantom tetromino
         * 
         * Requests sprites to be drawn for a phantom tetromino with the central block 
         * at (`x`,`y`) using tetromino coordinate system and computed block positions
         * 
         * @param x x coordinate [0,10]
         * @param y y coordinate [0,18]
         * @param block_type what tetromino this is based on `Tetromino_t`
         * @param spin_state what spin state tetromino is based on `Spin_t`
         * @return bool `false` if any of the blocks were OOB, `true` otherwise
         */
        bool drawPhantomTetromino(uint_fast8_t x, uint_fast8_t y, Tetromino_t block_type, Spin_t spin_state);

    private:
        SpritesController *_sprites_controller;
};