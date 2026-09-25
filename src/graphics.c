#include "graphics.h"
#include <zephyr/drivers/display.h>

// Framebuffer. Each pixel is monochrome, so 8 consecutive pixels are bundled per byte
static uint8_t fb[GFX_W * GFX_H / 8];

// Zephyr-driven display device
static const struct device *disp = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

// Display Buffer Descriptor
static struct display_buffer_descriptor desc = {
    .buf_size = sizeof(fb),
    .width    = GFX_W,
    .height   = GFX_H,
    .pitch    = GFX_W,
};

// Initializes graphics information and frame buffer using transposed coordinate system
int  gfx_init(void)
{
    if (!device_is_ready(disp)) {
        return -1;
    }

    gfx_clear();
    return 0;
}

// Clears the buffer by filling it with 0s
void gfx_clear(void)
{
    memset(fb, 0, sizeof(fb)); 
}

// Specifices state of individual pixel using transposed coordinate system
// x:  x coordinate
// y:  y coordinate
// on: pixel color (black or white)
bool gfx_px(uint_fast8_t x, uint_fast8_t y, bool on)
{
    // Framebuffer y coordinate's direction inside pages is reversed.
    // As such, we need to flip the last 8 bits to properly target the bit
    uint_fast8_t y_adjusted = y ^ 0b111;
    if (x >= GFX_W || y_adjusted >= GFX_H) return false;

    size_t idx  = (y_adjusted >> 3) * GFX_W + x;   /* which page, then which column */
    uint_fast8_t bit = BIT(y_adjusted & 7);           /* which row inside the page     */

    if (on) {
        fb[idx] |= bit;
    } else {
        fb[idx] &= ~bit;
    }

    return true;
}

// Draws a w-x-h unfilled rect at x and y of color on using transposed coordinate system
// x:  x coordinate
// y:  y coordinate
// w: width
// h: height
// on: pixel color (black or white)
void gfx_rect(uint_fast8_t x, uint_fast8_t y, uint_fast8_t w, uint_fast8_t h, bool on)
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
        gfx_px(i, y, on);
    }
    for (uint_fast8_t i = x; i < xpw; i++)
    {
        gfx_px(i, yphm1, on);
    }

    // Vertical lines
    for (uint_fast8_t j = y + 1; j < yphm1; j++)
    {
        gfx_px(x, j, on);
    }
    for (uint_fast8_t j = y + 1; j < yphm1; j++)
    {
        gfx_px(xpwm1, j, on);
    }
}

// Draws a w-x-h filled rect at x and y of color on using transposed coordinate system
// x:  x coordinate
// y:  y coordinate
// w: width
// h: height
// on: pixel color (black or white)
void gfx_fill(uint_fast8_t x, uint_fast8_t y, uint_fast8_t w, uint_fast8_t h, bool on)
{
    uint_fast8_t xpw = x + w;
    uint_fast8_t yph = y + h;

    for (uint_fast8_t i = x; i < xpw; i++)
    {
        for (uint_fast8_t j = y; j < yph; j++)
        {
            gfx_px(i,j,on);
        }
    }
}

// Flushes frame buffer to display
int  gfx_flush(void)
{
    display_write(disp, 0, 0, &desc, fb);
    display_blanking_off(disp);

    return 0;
}

// Draws playfield bounds
void gfx_playfield()
{
    gfx_rect(0, 0, 112, 64, true);
}