#include "decoder.hpp"

void DecoderController::initializeDecoder(const uint8_t fileId)
{    
    _inflateBuffer = _inflateBuffer_A;

    switch (fileId)
    {
        case 1:
            fileStartPointer = &(_binary_bapple_bin_start[0]);
            fileEndPointer = &(_binary_bapple_bin_end[0]);
            break;

        case 0:
        default:
            fileStartPointer = &(_binary_mainmenu_bin_start[0]);
            fileEndPointer = &(_binary_mainmenu_bin_end[0]);
            break;
    }

    currentPointer = fileStartPointer;
    numBlocks = *(currentPointer++);
    std::memcpy(&blockSize, currentPointer, sizeof(blockSize));
    currentPointer += 2;
    
}

void DecoderController::decodeBlock(bool checkBufferA)
{
    _inflateBuffer = checkBufferA ? _inflateBuffer_A : _inflateBuffer_B;

    if (fileStartPointer != nullptr)
    {
        _decoder->inflate_init(_inflateBuffer->data(), _inflateBuffer->size());
    
        _decoder->inflate(currentPointer, blockSize);
    
        currentPointer += blockSize;
        std::memcpy(&blockSize, currentPointer, sizeof(blockSize));
        currentPointer += 2;
        if (currentPointer >= fileEndPointer)
        {
            fileStartPointer = nullptr;
            fileEndPointer = nullptr;
        }
    }
}