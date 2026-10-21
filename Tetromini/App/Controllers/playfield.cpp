#include "playfield.hpp"

void PlayfieldController::setBlockState(uint8_t x, uint8_t y, uint8_t block_type)
{
    if (x >= 10 || y >= 22 || block_type > 7) { return; }

    uint32_t newValue = block_type << (x * 3);
    uint32_t tileMask = ~(0b111 << (x * 3));
    Playfield[y] = (Playfield[y] & tileMask) + newValue;
}

void PlayfieldController::renderPlayfield()
{
    // Draw playfield bounds
    _graphicsController->unfilledRect(0, 0, 112, 64, true);

    // Draw playfield itself
    for (size_t i = 0; i < 18; i++) // Last few rows are out of view, so we don't need to go through all of them!
    {
        uint32_t PlayfieldRow = Playfield[i+1];
        for (size_t j = 0; j < 10; j++) 
        {
            uint32_t tileMask = (0b111 << (j * 3));
            uint32_t tileData = (PlayfieldRow & tileMask) >> (j * 3);
            uint_fast8_t blockStyle = tileData & 0b111;
            _spritesController->drawBlock(j, i, blockStyle);
        }
    }

    // Draw phatom tetromino
    _tetrominoController->drawPhantomTetromino(pieceXPos, phantomYPos - 1, currentPiece, currentSpinState);

    // Draw current tetromino
    _tetrominoController->drawTetromino(pieceXPos, pieceYPos - 1, currentPiece, currentSpinState);
}

void PlayfieldController::renderPlayfieldHeader()
{
    
    _spritesController->drawNumber(121, 
        _spritesController->drawText(121, 1, "Score:"), 
        Score, 10);

    _spritesController->drawNumber(113, 
        _spritesController->drawText(113, 1, "Lvl:"), 
        Level, 10);


	_spritesController->drawText(113, 34, "Next:");

    _spritesController->drawSprite(70 + ((uint8_t)nextPiece), 113, 57);
}