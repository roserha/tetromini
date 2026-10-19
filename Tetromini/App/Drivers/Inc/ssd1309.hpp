#pragma once

#include <array>
#include <cstdint>
#include "main.h"
#include "FreeRTOS.h" // Semaphores for DMA framebuffer transfer
#include "semphr.h"   // Semaphores for DMA framebuffer transfer
#include "cmsis_os.h" // For delays

// Helper struct for GPIO pins
struct GPIOPin {
    GPIO_TypeDef*    port;
    uint16_t          pin;
};

// Class that contains methods and members for the OLED screen controller
class SSD1309Driver {
    public:
        static constexpr uint_fast8_t kScreenWidth  = 128;
        static constexpr uint_fast8_t kScreenHeight = 64;
        static constexpr uint16_t kFrameBufferSize = (kScreenWidth * kScreenHeight) / 8; 
        // The above should equal 1024 under initial 128x64 monochrome contiditons

        // Initialize member variables
        SSD1309Driver(SPI_HandleTypeDef* spi, std::array<uint8_t, kFrameBufferSize>* frame_buffer, GPIOPin chip_select, GPIOPin data_command, GPIOPin reset)
            : _spi(spi), _frame_buffer(frame_buffer), _chip_select(chip_select), _data_command(data_command), _reset(reset) {}

        /**
         * @brief Initialize SSD1309 controller. 
         * @warning REQUIRES MUTEX FOR FRAMEBUFFER!
         * 
         * Sends initialization commands to SSD1309 controller, creates 
         * semaphore for DMA, clears framebuffer and flushes empty frame to OLED.
         * 
         */
        bool init();

        /**
         * @brief Shut down SSD1309 controller
         * @warning REQUIRES MUTEX FOR FRAMEBUFFER!
         * 
         * Clears framebuffer, flushes empty frame to OLED, and
         * sends sleep command to OLED.
         * 
         */
        bool shutdown();

        // Wake up SSD1309 controller
        /**
         * @brief Wakes up SSD1309 controller
         * 
         * Sends wake-up command to SSD1309 controller
         * 
         */
        bool wakeup();

        /**
         * @brief Set all pixels of framebuffer to off
         * @warning REQUIRES MUTEX FOR FRAMEBUFFER!
         */
        void clear();

        /**
         * @brief Flush framebuffer to OLED through DMA
         * @warning REQUIRES MUTEX FOR FRAMEBUFFER!
         * 
         * Take semaphore for DMA, starts DMA transfer, wait for DMA callback
         * to give semaphore back, keeping OLED's CS at 0 until done.
         * 
         */
        bool flush();

        /**
         * @brief Send one single byte of a command to SSD1309
         * @param cmd Command to send 
         */
        bool command(uint8_t cmd) { return commands(&cmd, 1); }

        // Callback for DMA transfer being complete (ISR CONTEXT)
        void onTransferComplete();

        // Callback for DMA transfer ending in an error (ISR CONTEXT)
        void onTransferError();

        // Confirm if we currently own the SPI we reference in the callback
        bool ownsSpi(const SPI_HandleTypeDef* hspi) const { return hspi == _spi; }

    private:
        /**
         * @brief Abstraction of `send` to send commands to SSD1309
         * @note data_command pin LOW for command
         * @param bytes Pointer to array of bytes to send
         * @param len Length of bytes array
         */
        bool commands(const uint8_t* bytes, const uint16_t len);

        /**
         * @brief Abstraction of `send` to send data to SSD1309
         * @note data_command pin HIGH for data
         * @param bytes Pointer to array of bytes to send
         * @param len Length of bytes array
         */
        bool data(const uint8_t* bytes, const uint16_t len);

        /**
         * @brief Send either data or commands to SSD1309 depending on state of data_command pin
         * @param bytes Pointer to array of bytes to send
         * @param len Length of bytes array
         * @param dcLevel LOW for command, HIGH for data
         */
        bool send(const uint8_t* bytes, const uint16_t len, GPIO_PinState dcLevel);

        /**
         * @brief Write state to GPIO pin
         * @param pin GPIOPin to write state for
         * @param state what state to write
         */
        static void write(GPIOPin pin, GPIO_PinState state) { HAL_GPIO_WritePin(pin.port, pin.pin, state); }

        SPI_HandleTypeDef *_spi;
        std::array<uint8_t, kFrameBufferSize>* _frame_buffer;
        GPIOPin _chip_select, _data_command, _reset;

        StaticSemaphore_t _dmaDoneStorage; // static memory for the DMA semaphore
        SemaphoreHandle_t _dmaDone = nullptr; // given by the ISR when DMA finishes
        volatile bool _dmaFailed = false; // written in ISR, read in task
};