#include "graphics.hpp"


void GraphicsController::setPixel(const uint_fast8_t x, const uint_fast8_t y, const bool on)
{
    // Framebuffer y coordinate's direction inside pages is reversed.
    // As such, we need to flip the last 8 bits to properly target the bit
    if (x >= kScreenWidth || y >= kScreenHeight) return;

    const size_t idx  = (y >> 3) * kScreenWidth + x;   /* which page, then which column */
    const uint_fast8_t bit = 1u << (y & 7);           /* which row inside the page     */

    if (xSemaphoreTake(*_xFrameBufferMutex, 10)) // Wait for 10 ticks at most
    {
        if (on) {
            (*_frame_buffer)[idx] |= bit;
        } else {
            (*_frame_buffer)[idx] &= ~bit;
        }

        xSemaphoreGive(*_xFrameBufferMutex);
    }

    return;
}

void GraphicsController::unfilledRect(uint_fast8_t x, uint_fast8_t y, uint_fast8_t w, uint_fast8_t h, bool on)
{
    if (w == 0 || h == 0)
    {
        return;
    }

    uint_fast8_t xpw = x + w;
    uint_fast8_t xpwm1 = x + w - 1U;
    uint_fast8_t yphm1 = y + h - 1U;

    // Horizontal lines
    for (uint_fast8_t i = x; i < xpw; i++)
    {
        setPixel(i, y, on);
    }
    for (uint_fast8_t i = x; i < xpw; i++)
    {
        setPixel(i, yphm1, on);
    }

    // Vertical lines
    for (uint_fast8_t j = y + 1; j < yphm1; j++)
    {
        setPixel(x, j, on);
    }
    for (uint_fast8_t j = y + 1; j < yphm1; j++)
    {
        setPixel(xpwm1, j, on);
    }
}

void GraphicsController::filledRect(uint_fast8_t x, uint_fast8_t y, uint_fast8_t w, uint_fast8_t h, bool on)
{
    uint_fast8_t xpw = x + w;
    uint_fast8_t yph = y + h;

    for (uint_fast8_t i = x; i < xpw; i++)
    {
        for (uint_fast8_t j = y; j < yph; j++)
        {
            setPixel(i,j,on);
        }
    }
}