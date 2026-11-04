#include "video.hpp"

bool VideoController::displayVideoFrame(std::array<uint8_t, 36872> *videoBuffer, uint16_t videoFrame)
{
    // Skip if we'd pass thru frame
    if (videoFrame*sizeof(*_frame_buffer) > sizeof(*videoBuffer))
        return false;

    // Video frames are stored in the same way as the framebuffer!
    if (videoFrame == 0)
    {
        // So for the first frame, we can memcpy,
        if (xSemaphoreTake(*_xFrameBufferMutex, 10)) // Wait for 10 ticks at most
        {
            std::memcpy(_frame_buffer->data(), videoBuffer->data() + videoFrame*GraphicsController::kFrameBufferSize, GraphicsController::kFrameBufferSize);
            xSemaphoreGive(*_xFrameBufferMutex);
        }
    } else
    {
        // And for other frames we just loop XORring frames together!
        for (int i = 0; i < GraphicsController::kFrameBufferSize; i++)
        {
            if ((videoFrame*GraphicsController::kFrameBufferSize) + i >= videoBuffer->size())
                break;

            if (xSemaphoreTake(*_xFrameBufferMutex, 10)) // Wait for 10 ticks at most
            {
                (*_frame_buffer)[i] ^= (*videoBuffer)[(videoFrame*GraphicsController::kFrameBufferSize) + i];
                xSemaphoreGive(*_xFrameBufferMutex);
            }
        }
    }

    return true;
}