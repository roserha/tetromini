#include "decoder.hpp"

void DecoderController::initializeDecoder(const uint8_t fileId)
{    
    _inflateBuffer = _inflateBuffer_A;

    switch (fileId)
    {
        case 0:
        default:
            fileStartPointer = &(_binary_bapple_bin_start[0]);
    }

    currentPointer = fileStartPointer;
    numBlocks = *(currentPointer++);
    std::memcpy(&blockSize, currentPointer, sizeof(blockSize));
    currentPointer += 2;
    
}

void DecoderController::decodeBlock()
{
    //decoder.inflate()
    _decoder->inflate_init(_inflateBuffer->data(), _inflateBuffer->size());

    _decoder->inflate(currentPointer, blockSize);

    inflateBufferPointsB = !inflateBufferPointsB;
    _inflateBuffer = inflateBufferPointsB ? _inflateBuffer_B : _inflateBuffer_A;

    currentPointer += blockSize;
    std::memcpy(&blockSize, currentPointer, sizeof(blockSize));
    currentPointer += 2;
}