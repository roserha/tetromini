#include "ssd1309.hpp"

namespace {

    SSD1309Driver* g_currentControllerInstance = nullptr;

    constexpr uint32_t kSpiTimeoutMs = 100;

    // Initialization commands for SSD1309, cross-checked from old dts file from zephyr
    constexpr uint8_t kSpiInitSequence[] = {
        0xAE,         // display off (sleep) while configuring
        0xD5, 0xA0,   // clock: osc freq setting 10, divide ratio 1
        0xA8, 0x3F,   // multiplex ratio: 64 rows (value = rows - 1)
        0xD3, 0x00,   // display offset: no vertical shift
        0x40,         // display start line: row 0 (0x40 | line)
        0x20, 0x00,   // addressing mode: horizontal (auto-wraps to next page)
        0xA1,         // segment remap: mirrored L/R (0xA0 = normal)
        0xC8,         // COM scan: flipped vertically (0xC0 = normal)
        0xDA, 0x12,   // COM pins: alternative layout, no L/R remap
        0x81, 0x7F,   // contrast: 0x7F of 0xFF (higher = brighter)
        0xD9, 0x22,   // pre-charge: phase 1 = 2 clocks, phase 2 = 2 clocks
        0xDB, 0x34,   // VCOMH deselect level: ~0.78 x VCC
        0xA4,         // pixels follow RAM (0xA5 = all on, smoke test)
        0xA6,         // normal display (0xA7 = inverted)
        0xAF,         // display on
        // no 0x8D charge pump: SSD1309 doesn't have one
    };

    // Commands to address the screen itself
    constexpr uint8_t kWindowSequence[] = {
        0x21, 0, 127,        // column range: 0..127
        0x22, 0, 7,   // page range: 0..7 (8 rows per page)
    };

}

bool SSD1309Driver::init()
{
    write(_chip_select, GPIO_PIN_SET);                         // Deselect chip
    write(_reset, GPIO_PIN_RESET);                             // Hold display in reset mode
    osDelay(10);                                               // Wait 10 ticks
    write(_reset, GPIO_PIN_SET);                               // Stop resetting
    osDelay(100);                                              // Wait 100 ticks for display to wake up
    const bool properInit = commands(kSpiInitSequence, sizeof(kSpiInitSequence)); // Send init sequence

    if (!properInit)
        return false;

    // clear buffer and oled controller's ram by flushing empty framebuffer
    _dmaDone = xSemaphoreCreateBinaryStatic(&_dmaDoneStorage);   // starts empty
    configASSERT(_dmaDone != nullptr);
    clear();
    const bool clearOk = flush();  // push 1024 zero bytes to the OLED's RAM

    return clearOk;
}

bool SSD1309Driver::shutdown()
{
    clear();
    bool flushOk = flush();
    bool shutdownOk = command(0xAE);

    return shutdownOk && flushOk;
}

bool SSD1309Driver::wakeup()
{
    return command(0xAF);
}

void SSD1309Driver::clear()
{
    _frame_buffer->fill(0);
}


bool SSD1309Driver::flush()
{
    bool beginWriting = commands(kWindowSequence, sizeof(kWindowSequence)); 
    
    if (!beginWriting) // Don't take a semaphore if there was an issue asking to write!
        return false;
    
    // Take DMA semaphore
    xSemaphoreTake(_dmaDone, 0); 
    _dmaFailed = false;
    g_currentControllerInstance = this;

    write(_data_command, GPIO_PIN_SET);
    write(_chip_select, GPIO_PIN_RESET);


    const HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(_spi, const_cast<uint8_t*>(_frame_buffer->data()), kFrameBufferSize);

    // Fallback if transfer doesn't work
    if (status != HAL_OK)
    {
        _dmaFailed = true;
        write(_chip_select, GPIO_PIN_SET);
    }
    
    // Sleep render task until we're done or until it times out!
    if (xSemaphoreTake(_dmaDone, kSpiTimeoutMs) != pdTRUE) {
        // We timed out!!! :( Let's handle that
        HAL_SPI_Abort(_spi);                // timed out: stop the DMA
        write(_chip_select, GPIO_PIN_SET);
        return false;
    }

    return (status == HAL_OK) && !_dmaFailed;
}

void SSD1309Driver::onTransferComplete()
{
    write(_chip_select, GPIO_PIN_SET);

    // wake flush() up by giving to the semaphore
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(_dmaDone, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

void SSD1309Driver::onTransferError()
{
    _dmaFailed = true;
    onTransferComplete(); // still deselect + wake flush()
}

bool SSD1309Driver::commands(const uint8_t* bytes, const uint16_t len)
{
    return send(bytes, len, GPIO_PIN_RESET);
}

bool SSD1309Driver::data(const uint8_t* bytes, const uint16_t len)
{
    return send(bytes, len, GPIO_PIN_SET);    
}

bool SSD1309Driver::send(const uint8_t* bytes, const uint16_t len, GPIO_PinState dcLevel)
{
    write(_data_command, dcLevel);
    write(_chip_select, GPIO_PIN_RESET);

    // Transmit whichever bytes we want via SPI thru HAL
    const HAL_StatusTypeDef status = HAL_SPI_Transmit(_spi, const_cast<uint8_t*>(bytes), len, kSpiTimeoutMs);

    write(_chip_select, GPIO_PIN_SET);

    return status == HAL_OK;
}

extern "C" void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi)
{
    if (g_currentControllerInstance != nullptr && g_currentControllerInstance->ownsSpi(hspi)) {
        g_currentControllerInstance->onTransferComplete();
    }
}

extern "C" void HAL_SPI_ErrorCallback(SPI_HandleTypeDef* hspi)
{
    if (g_currentControllerInstance != nullptr && g_currentControllerInstance->ownsSpi(hspi)) {
        g_currentControllerInstance->onTransferError();
    }
}