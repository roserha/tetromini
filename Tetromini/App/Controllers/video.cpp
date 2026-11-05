#include "video.hpp"

bool VideoController::displayVideoFrame(std::array<uint8_t, 36872> *videoBuffer, uint16_t videoFrame)
{
    size_t offset = videoFrame*GraphicsController::kFrameBufferSize;

    // Skip if we'd pass thru buffer at any point
    if (offset + GraphicsController::kFrameBufferSize > 36872)
        return false;

    if (xSemaphoreTake(*_xFrameBufferMutex, 10)) // Wait for 10 ticks at most
    {
        // Video frames are stored in the same way as the framebuffer!
        std::memcpy(_frame_buffer->data(), videoBuffer->data() + offset, GraphicsController::kFrameBufferSize);
        xSemaphoreGive(*_xFrameBufferMutex);
    }

    return true;
}