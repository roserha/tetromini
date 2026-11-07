#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include "main.h"
#include "zlib_turbo.h"  // Access turbo zlib methods
#include "FreeRTOS.h"   // FreeRTOS necessities
#include "semphr.h"     // Mutexes!
#include "graphics.hpp"

class VideoController {
    public:
        VideoController(std::array<uint8_t, GraphicsController::kFrameBufferSize>* frame_buffer, SemaphoreHandle_t* xFrameBufferMutex):
            _frame_buffer(frame_buffer), _xFrameBufferMutex(xFrameBufferMutex) {}

        /**
         * @brief Display video frame `videoFrame` from `videoBuffer`
         * Copy frame `videoFrame`'s data from `videoBuffer` into the system's framebuffer.
         * @param videoBuffer current block of buffer we're reading
         * @param videoFrame what frame inside block we're copying
         * @return bool true if frame was successfully copied, false otherwise (when we ran out of frames)
         */
        bool displayVideoFrame(std::array<uint8_t, 36872> *videoBuffer, uint16_t videoFrame);

    private:
        bool inflateBufferPointsB = false;
        uint8_t const* fileStartPointer;
        uint8_t const* currentPointer;
        uint8_t currentIndex = 0;
        uint16_t blockSize = 0;
        uint8_t numBlocks = 0;

        // Framebuffer. Each pixel is monochrome, so 8 consecutive pixels are bundled per byte
        std::array<uint8_t, GraphicsController::kFrameBufferSize>* _frame_buffer;

        // Mutex for the frame buffer itself.
        SemaphoreHandle_t* _xFrameBufferMutex;
};