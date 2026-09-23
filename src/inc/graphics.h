#ifndef GRAPHICS
#define GRAPHICS

/* 
 * Physical panel is 128x64 landscape. the console is held transposed 90 deg,
 * so x runs along the panel's long axis = VERTICAL as the player sees it.
 * x=0 is the top of the playfield, y=0 is the left edge.
 */

 // Given the color resolution, our frame buffer should occupy ~1.024KB of SRAM, since
 // each pixel is stored as a single bit instead of a byte!

#include <stdbool.h>
#include <stdint.h>

#define GFX_W 128
#define GFX_H 64

// Initializes graphics information and frame buffer using transposed coordinate system
int  gfx_init(void);

// Clears the buffer
void gfx_clear(void);

// Specifices state of individual pixel using transposed coordinate system
// x:  x coordinate
// y:  y coordinate
// on: pixel color (black or white)
void gfx_px(uint_fast8_t x, uint_fast8_t y, bool on);

// Draws a w-x-h unfilled rect at x and y of color on using transposed coordinate system
// x:  x coordinate
// y:  y coordinate
// w: width
// h: height
// on: pixel color (black or white)
void gfx_rect(uint_fast8_t x, uint_fast8_t y, uint_fast8_t w, uint_fast8_t h, bool on);

// Draws a w-x-h filled rect at x and y of color on using transposed coordinate system
// x:  x coordinate
// y:  y coordinate
// w: width
// h: height
// on: pixel color (black or white)
void gfx_fill(uint_fast8_t x, uint_fast8_t y, uint_fast8_t w, uint_fast8_t h, bool on);

// Flushes frame buffer to display
int  gfx_flush(void);

// Draws playfield bounds
void gfx_playfield();

#endif