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

Tetromino_t PlayfieldController::pullNewPiece()
{
    bool foundPiece = false;
    Tetromino_t piece = TBlock;

    if (seen_pieces == 0b1111111)
    {
        // We've seen all pieces, let's refresh bag
        seen_pieces = 0;
    }

    while (!foundPiece)
    {
        // Pseudo-random number generator
        uint64_t rand = (3739 * stopwatch + *_salt) & 0b1111111;
        *_salt += 14929;

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
                    piece = (Tetromino_t)(i+1);
                    seen_pieces |= pieceCheckIndex;
                    break;
                }
            }
        }
    }

    return piece;
}

bool PlayfieldController::checkForCollision(Tetromino_t block_type, Spin_t spin_state, uint_fast8_t x, uint_fast8_t y)
{
    std::array<uint8_t, 8U> Coordinates = _tetrominoController->getTetrominoPositions(x, y, block_type, spin_state);

    // Simpler collision: colliding with walls (horizontally or bottom)
    for (int i = 0; i < 8; i++)
    {
        if ((i & 1) == 0) // Checking x (i is even)
        {
            if (Coordinates[i] >= 10)
            {
                return true;
            }
        }
        else // Checking y (i is odd)
        {
            if (Coordinates[i] == 0)
            {
                return true;
            }
        }
    }

    // Harder collision: colliding with existing playfields
    // Transform x and y array into array coordinates

    for (int i = 0; i < 8; i+=2)
    {
        // Check if coordinate we're looking at is set to occupied

        size_t y = Coordinates[i+1];
        uint8_t x = Coordinates[i];

        uint32_t rowToCheck = Playfield[y];
        uint32_t tileMask = (0b111 << (x * 3));

        if ((rowToCheck & tileMask) != 0)
        {
            return true;
        }
    }

    // No collisions here!
    return false;
}

void PlayfieldController::rotateCurrentPiece(bool right)
{
    Spin_t desiredSpinState = currentSpinState;

    switch (currentSpinState)
    {
        case ZeroDeg:
            desiredSpinState = right ? TwoSeventyDeg : NinetyDeg;
            break;

        case TwoSeventyDeg:
            desiredSpinState = right ? OneEightyDeg : ZeroDeg;
            break;

        case OneEightyDeg:
            desiredSpinState = right ? NinetyDeg : TwoSeventyDeg;
            break;

        default:
        case NinetyDeg:
            desiredSpinState = right ? ZeroDeg : OneEightyDeg;
            break;
    }

    // Wall kick detection logic
    if (currentPiece == OBlock)
    {
        int_fast8_t targXKick = kO_Offsets[desiredSpinState*2 + 0] + 1; int_fast8_t targYKick = kO_Offsets[desiredSpinState*2 + 1] + 1;
        int_fast8_t prevXKick = kO_Offsets[currentSpinState*2 + 0] + 1; int_fast8_t prevYKick = kO_Offsets[currentSpinState*2 + 1] + 1;

        pieceXPos += prevXKick - targXKick;
        pieceYPos += prevYKick - targYKick;
        currentSpinState = desiredSpinState;
        updatePhantomPiecePosition();
    }
    else
    {
        const std::array<int8_t, 40>* referenceTable = (currentPiece == IBlock) ? &kI_Offsets : &kJLSTZ_Offsets;
        
        for (int i = 0; i < 10; i+=2)
        {
            int_fast8_t prevXKick = (*referenceTable)[currentSpinState*10 + i]; int_fast8_t prevYKick = (*referenceTable)[currentSpinState*10 + i + 1];
            int_fast8_t targXKick = (*referenceTable)[desiredSpinState*10 + i]; int_fast8_t targYKick = (*referenceTable)[desiredSpinState*10 + i + 1];

            bool willItCollide = checkForCollision(currentPiece, desiredSpinState, pieceXPos + (prevXKick - targXKick), pieceYPos + (prevYKick - targYKick));
            if (!willItCollide)
            {
                pieceXPos += prevXKick - targXKick;
                pieceYPos += prevYKick - targYKick;
                currentSpinState = desiredSpinState;
                updatePhantomPiecePosition();
                break;
            }
        }
    }
}

void PlayfieldController::moveCurrentPiece(bool right)
{
    bool willItCollide = checkForCollision(currentPiece, currentSpinState, pieceXPos + (right ? 1 : -1), pieceYPos);
    if (!willItCollide)
    {
        pieceXPos += right ? 1 : -1;
        updatePhantomPiecePosition();
    }
}

void PlayfieldController::finishDroppingPiece()
{
    // Set playfield to have new blocks
    std::array<uint8_t, 8U> Coordinates = _tetrominoController->getTetrominoPositions(pieceXPos, pieceYPos, currentPiece, currentSpinState);

    // Set these blocks to be active in their respective color
    for (int i = 0; i < 8; i+=2)
    {
        setBlockState(Coordinates[i], Coordinates[i+1], currentPiece);
    }

    currentPiece = nextPiece;
    currentSpinState = ZeroDeg;
    pieceYPos = 20;
    if (currentPiece == OBlock)
    {
        pieceXPos = 5;
    }
    else
    {
        pieceXPos = 4;
    }
    nextPiece = pullNewPiece();

    checkForLineClear = true;

    updatePhantomPiecePosition();
}

void PlayfieldController::softDropCurrentPiece()
{
    bool willItCollide = checkForCollision(currentPiece, currentSpinState, pieceXPos, pieceYPos - 1);
        
    if (!willItCollide)
    {
        pieceYPos--;
    }
    else
    {
        finishDroppingPiece();
    } 
}

void PlayfieldController::hardDropCurrentPiece()
{
    pieceYPos = phantomYPos;
    finishDroppingPiece();
}

void PlayfieldController::setSoftDropStatus(bool status)
{
    softDropping = status;
}

void PlayfieldController::updatePhantomPiecePosition()
{
    for (int y = pieceYPos; y > 0; y--)
    {
        bool willItCollide = checkForCollision(currentPiece, currentSpinState, pieceXPos, y - 1);
        
        if (willItCollide)
        {
            phantomYPos = y;
            break;
        }
    }
}

void PlayfieldController::initializePlayfield()
{
    // Initialize us at a random state of seen pieces
    uint64_t rand = (3739 * stopwatch + *_salt);
    seen_pieces = rand & 0b1111111;
    
    currentPiece = pullNewPiece();
    nextPiece = pullNewPiece();
    updatePhantomPiecePosition();
}

void PlayfieldController::tickPlayfield(uint32_t delta_time, uint32_t elapsed_time)
{
    // Skip first frame
    if (elapsed_time == 0)
    {
        return;
    }

    // Drop piece based on if we're either regular, soft, or hard dropping
    if (!softDropping)
    {
        if (elapsed_time - stopwatch > PieceDropTimeout && !checkForLineClear)
        {
            softDropCurrentPiece();
            stopwatch = elapsed_time;
        }
    } else if (phantomYPos == pieceYPos)
    {
        if (elapsed_time - (stopwatch << 2) > PieceDropTimeout && !checkForLineClear)
        {
            softDropCurrentPiece();
            stopwatch = elapsed_time;
        }
    }
    else
    {
        if (elapsed_time - (stopwatch >> 2) > PieceDropTimeout && !checkForLineClear)
        {
            softDropCurrentPiece();
            stopwatch = elapsed_time;
        }
    }

    // LINE CLEAR LOGIC
    // Check for line clear triggers line checking
    if (checkForLineClear)
    {        
        // Reset indexes to shift
        indexesToShift.fill(0);

        uint_fast8_t linesCleared = 0;

        // Walk through each line to confirm whether we have a line clear
        for (int i = 0; i < 22; i++)
        {
            bool wholeLineOccupied = true;
            uint32_t PlayfieldRow = Playfield[i];

            for (int j = 0; j < 10; j++)
            {
                uint32_t tileMask = (0b111 << (j * 3));
                
                wholeLineOccupied &= ((PlayfieldRow & tileMask) != 0);
                
                if (!wholeLineOccupied)
                {
                    // Let's already skip to next line
                    break;
                }

                if (wholeLineOccupied && j == 9)
                {
                    // WE HAVE A LINE CLEAR!
                    indexesToShift[linesCleared] = i - linesCleared; // <- we have to remember that the line in question will be 1 row down when prev line is cleared
                    linesCleared += 1;
                    stopwatch = elapsed_time;
                }
            }
        }

        checkForLineClear = (linesCleared != 0);

        // Sound effects
        Command_t sfx = {CMD_AUDIO_PLAY_SOUND, 0, nullptr};
        if (!checkForLineClear)
        {
            sfx.cmdInfo = AudioController::sampleToUlValue(4, 2, 6, 20, 128);
            xQueueSend(_audioQueue, (void *)&sfx, 0);
        } else
        {
            switch (linesCleared)
            {
                case 1:
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 2, 6, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    vTaskDelay(pdMS_TO_TICKS(128));
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 5, 6, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    break;

                case 2:
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 2, 6, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    vTaskDelay(pdMS_TO_TICKS(128));
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 5, 6, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    vTaskDelay(pdMS_TO_TICKS(128));
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 9, 6, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    break;

                case 3:
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 2, 6, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    vTaskDelay(pdMS_TO_TICKS(128));
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 5, 6, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    vTaskDelay(pdMS_TO_TICKS(128));
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 9, 6, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    vTaskDelay(pdMS_TO_TICKS(128));
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 0, 7, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    break;

                case 4:
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 2, 6, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    vTaskDelay(pdMS_TO_TICKS(128));
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 5, 6, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    vTaskDelay(pdMS_TO_TICKS(128));
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 9, 6, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    vTaskDelay(pdMS_TO_TICKS(128));
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 5, 6, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    vTaskDelay(pdMS_TO_TICKS(128));
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 9, 6, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    vTaskDelay(pdMS_TO_TICKS(128));
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 0, 7, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    vTaskDelay(pdMS_TO_TICKS(128));
                    sfx.cmdInfo = AudioController::sampleToUlValue(4, 4, 7, 20, 128);
                    xQueueSend(_audioQueue, (void *)&sfx, 0);
                    break;
                
                default:
                    break;
            }
        }

        // Actually add points and clear lines
        if (checkForLineClear)
        {
            switch (linesCleared)
            {
                case 1: Score += 100 * Level;  break;
                case 2: Score += 400 * Level;  break;
                case 3: Score += 900 * Level;  break;
                case 4: Score += 1600 * Level; break;
                default:                       break;
            }

            Score += 50 * (Combo++) * Level;

            TotalLinesCleared += linesCleared * linesCleared;

            while (TotalLinesCleared > Level * 10)
            {
                TotalLinesCleared -= Level * 10;
                Level++;
                if (Level <= 8)
                    PieceDropTimeout -= 83;
                else if (Level == 9)
                    PieceDropTimeout -= 33;
                else if (Level == 10)
                    PieceDropTimeout -= 17;
                else if (Level <= 19)
                    PieceDropTimeout -= 5;
                else if (Level <= 28)
                    PieceDropTimeout -= 2;
            }

            // We are only at most clearing 4 lines at a time, so the translation array will only
            // have to look at a depth of 4. Unused spaces are labeled as 0, so when we find a 0,
            // we stop.
            
            for (int i = 0; i < 4; i++)
            {
                uint_fast8_t index = indexesToShift[i];
                if (index == 0)
                {
                    break;
                }
                
                std::move(Playfield.begin() + index + 1, Playfield.end(), Playfield.begin() + index);
                Playfield[21] = 0;
            }
            
            checkForLineClear = false;
        }
        else
            Combo = 1;

    }

}