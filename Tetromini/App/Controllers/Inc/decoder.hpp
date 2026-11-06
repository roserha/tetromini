#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include "main.h"
#include "zlib_turbo.h"  // Access turbo zlib methods
#include "FreeRTOS.h"   // FreeRTOS necessities
#include "semphr.h"     // Mutexes!

extern "C" const uint8_t _binary_bapple_bin_start[], _binary_bapple_bin_end[];
extern "C" const uint8_t _binary_mainmenu_bin_start[], _binary_mainmenu_bin_end[];

class DecoderController {
    public:
        DecoderController(zlib_turbo *dec, std::array<uint8_t, 36872> *inflateBufferA, std::array<uint8_t, 36872> *inflateBufferB):
            _decoder(dec), _inflateBuffer_A(inflateBufferA), _inflateBuffer_B(inflateBufferB) {}

        void initializeDecoder(const uint8_t fileId);
        
        void decodeBlock(bool checkBufferA);

    private:
        zlib_turbo *_decoder;
        std::array<uint8_t, 36872> *_inflateBuffer_A;
        std::array<uint8_t, 36872> *_inflateBuffer_B;
        std::array<uint8_t, 36872> *_inflateBuffer;
        bool inflateBufferPointsB = false;
        uint8_t const* fileStartPointer;
        uint8_t const* fileEndPointer;
        uint8_t const* currentPointer;
        uint8_t currentIndex = 0;
        uint16_t blockSize = 0;
        uint8_t numBlocks = 0;
};