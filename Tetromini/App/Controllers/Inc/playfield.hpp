#pragma once
#include "tetromino.hpp"
#include <cstdint>
#include <array>

class PlayfieldController {
    public:
        // Initialize default variables
        PlayfieldController(TetrominoController* tetrominoController, SpritesController* spritesController, GraphicsController* graphicsController)
            : _tetrominoController(tetrominoController), _spritesController(spritesController), _graphicsController(graphicsController) {}

        /**
         * @brief Set block state on bitfield
         * 
         * Set state of block on bitfield based on tetrimino-coordinates
         * 
         * @param x x coordinate [0,10]
         * @param y y coordinate [0,24)
         * @param on pixel color (true = white, false = black)
         */
        void setBlockState(uint8_t x, uint8_t y, uint8_t block_type);

        /**
         * @brief Render active playfield
         * 
         * Update framebuffer to reflect current playfield state
         * 
         */
        void renderPlayfield();

        /**
         * @brief Renders current info (score, level, next piece)
         * 
         * Update framebuffer to reflect current info (score, level, next piece)
         * 
         */
        void renderPlayfieldHeader();

        /**
         * @brief Pull a random tetromino from the bag
         * 
         * Request an unseen piece from the "bag". If we have seen all pieces, reshuffle
         * the bag and get a new piece.
         * 
         */
        Tetromino_t pullNewPiece();

        /**
         * @brief Check for collision of a hypothetical tetromino
         * 
         * Given a `block_type` tetromino, see if putting it with its center at (`x`,`y`) and
         * `spin_state` spin state within the current playfield would yield a collision.
         * 
         * @param block_type What kind of `Tetromino_t` the block in question is
         * @param spin_state What the `Spin_t` of the block in question is
         * @param x x coordinate [0,10]
         * @param y y coordinate [0,24)
         * 
         * @return bool `true` if a collision happens, `false` otherwise
         */
        bool checkForCollision(Tetromino_t block_type, Spin_t spin_state, uint_fast8_t x, uint_fast8_t y);

        /**
         * @brief Rotate the tetromino clockwise or counterclockwise
         * 
         * Update the currently held tetromino to rotate either clockwise
         * or counterclockwise
         * 
         * @param clockwise `true` for clockwise, `false` for counterclockwise
         */
        void rotateCurrentPiece(bool clockwise);

        /**
         * @brief Move the tetromino right or left
         * 
         * Update the currently held tetromino to move either to the right
         * or to the left
         * 
         * @param right `true` for to the right, `false` for to the left
         * @note take it back now yall :danceemoji:
         */
        void moveCurrentPiece(bool right);

        /**
         * @brief Softly drop the piece
         * 
         * Move the piece down faster than what the level is, without
         * immediately snapping to phantom piece position
         */
        void softDropCurrentPiece();

        /**
         * @brief Hard-drop the piece
         * 
         * Snap the piece down to the phantom piece's position
         */
        void hardDropCurrentPiece();

        /**
         * @brief Update phantom piece position
         * 
         * Go down from the current piece's y to see when the phantom piece
         * would collide, and update its y position to that.
         */
        void updatePhantomPiecePosition();

        /**
         * @brief Initializes playfield variables
         * 
         * Set variables to their default values
         */
        void initializePlayfield();

        /**
         * @brief Update current playfield status
         * 
         * Compute every single logistic about the current state
         * of the playfield given the currently passed time
         * @param delta_time time passed in-between frames
         * @param elapsed_time time passed since boot
         */
        // 
        void tickPlayfield(int64_t delta_time, int64_t elapsed_time, bool *user_data);

    private:
        // We will keep track of the active playfield by borrowing bitboards from chess!!
        // Given the playfield is 10x22, with only the first 18 rows visible, we need to store 220 blocks of data. 
        // (row 0 needs to exist because otherwise floor collisions have the chance to underflow and undefined things happen :p sue me but not really)
        // With 6 blocks, we can store a tile's blocktype, and naturally b000 means empty!
        // With 10 tiles per row, this means 30 bits per row! Each row can be represented by a 32-bit int, wasting only 2 bits per row!
        // As such, with 22 rows, our entire playfield takes up only 88 bytes!! RAH!!!!! EFFICIENCY!!!!!!!!!
        uint32_t Playfield [22] = {0};

        // Current score
        uint32_t Score = 0;

        // Current level
        uint_fast8_t Level = 1;

        // How many lines we've cleared
        uint32_t TotalLinesCleared = 0;

        // Current combo
        uint32_t Combo = 0;

        // How much time needs to pass for piece to drop
        uint32_t PieceDropTimeout = 717;

        // Bitmask of the pieces we've seen
        uint_fast8_t seen_pieces = 0;

        // Next piece to show up
        Tetromino_t nextPiece = TBlock;

        // Current piece type
        Tetromino_t currentPiece = TBlock;

        // Current piece rotation state
        Spin_t currentSpinState = ZeroDeg;

        // Current piece's x position
        uint_fast8_t pieceXPos = 7;

        // Current piece's y position
        uint_fast8_t pieceYPos = 20;

        // Current phantom piece's y position
        uint_fast8_t phantomYPos = 20;

        // Internal clock of how many ms passed since game start
        int64_t stopwatch = 0;

        // Have we checked the playfield for line cleards? Stays true if we have a line clear
        bool checkForLineClear = false;

        // Whether we have finished checking for line clears or not
        bool finishedCheck = false;

        // Indexes of playfield to shift when line clearing
        std::array<uint_fast8_t, 4> indexesToShift = {0};

        // Piece offset data
        // This comes from the SRS page in the Tetris Wiki.
        // The way pieces are represented made it such that the naturally best approach to deal with rotations
        // and wall-kicks are by using offsets, since their rotations are naturally True Rotations, given their
        // root on vector and rotational matrix multiplication!

        // Sadly, we have to store them all explicitly to make computation easier... I'm sorry, stack :v
        static constexpr std::array<int8_t, 40> kJLSTZ_Offsets = {
        0, 0,    0, 0,    0, 0,    0, 0,    0, 0,
        0, 0,    1, 0,    1,-1,    0, 2,    1, 2,
        0, 0,    0, 0,    0, 0,    0, 0,    0, 0,
        0, 0,   -1, 0,   -1,-1,    0, 2,   -1, 2
        };

        static constexpr std::array<int8_t, 40> kI_Offsets = {
            0, 0,   -1, 0,    2, 0,   -1, 0,    2, 0,
           -1, 0,    0, 0,    0, 0,    0, 1,    0,-2,
           -1, 1,    1, 1,   -2, 1,    1, 0,   -2, 0,
            0, 1,    0, 1,    0, 1,    0,-1,    0, 2
        };

        // These values are either false for -1 and true for 0 (bc i can just do O_Offset[i] + 1)
        // This is my way to at least save SOME space.
        static constexpr std::array<bool, 8> kO_Offsets = {
            true, true,
            true,false,
            false,false,
            false, true
        };

        // Instance of Tetromino Controller
        TetrominoController* _tetrominoController;
        
        // Instance of Sprites Controller
        SpritesController* _spritesController;
        
        // Instance of Graphics Controller
        GraphicsController* _graphicsController;
};