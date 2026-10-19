#pragma once

#include <array>
#include <cstdint>
#include "main.h"
#include "ssd1309.hpp"  // Access oled variables
#include "FreeRTOS.h"   // FreeRTOS necessities
#include "semphr.h"     // Mutexes!



// Class that contains methods and members for the OLED screen controller
class GraphicsController {
    public:
        static constexpr uint_fast8_t kScreenWidth  = SSD1309Driver::kScreenWidth;
        static constexpr uint_fast8_t kScreenHeight = SSD1309Driver::kScreenHeight;
        static constexpr uint16_t kFrameBufferSize = SSD1309Driver::kFrameBufferSize;

        // Initialize member variables
        GraphicsController(std::array<uint8_t, kFrameBufferSize>* frame_buffer, SemaphoreHandle_t* xFrameBufferMutex)
            : _frame_buffer(frame_buffer), _xFrameBufferMutex(xFrameBufferMutex) {}

        /**
         * @brief Set pixel color
         * @note This method automatically requests and processes a mutex to alter the framebuffer! 
         *       As such, any other methods from broader classes don't require mutexes.
         * 
         * Specificies state of individual pixel using transposed 
         * coordinate system
         * 
         * @param x x coordinate [0,128)
         * @param y y coordinate [0,64)
         * @param on pixel color (true = white, false = black)
         */
        void setPixel(const uint_fast8_t x, const uint_fast8_t y, const bool on);

        /**
         * @brief Draw unfilled rect
         * 
         * Draws a `w`x`h` unfilled rect at `x` and `y` of color `on` 
         * using transposed coordinate system
         * 
         * @param x x coordinate [0,128)
         * @param y y coordinate [0,64)
         * @param w width
         * @param h height
         * @param on pixel color (true = white, false = black)
         */
        void unfilledRect(uint_fast8_t x, uint_fast8_t y, uint_fast8_t w, uint_fast8_t h, bool on);

        /**
         * @brief Draw filled rect
         * 
         * Draws a `w`x`h` filled rect at `x` and `y` of color `on` 
         * using transposed coordinate system
         * 
         * @param x x coordinate [0,128)
         * @param y y coordinate [0,64)
         * @param w width
         * @param h height
         * @param on pixel color (true = white, false = black)
         */
        void filledRect(uint_fast8_t x, uint_fast8_t y, uint_fast8_t w, uint_fast8_t h, bool on);

    private:
        // Framebuffer. Each pixel is monochrome, so 8 consecutive pixels are bundled per byte
        std::array<uint8_t, kFrameBufferSize>* _frame_buffer;

        // Mutex for the frame buffer itself.
        SemaphoreHandle_t* _xFrameBufferMutex;
};