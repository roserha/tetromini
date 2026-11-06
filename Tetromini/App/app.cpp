#include "app.h"

#include "main.h"        // HAL + pin labels (Blinky_Pin, etc.)
#include "FreeRTOS.h"    // FreeRTOS necessities
#include "semphr.h"      // Mutexes!
#include "task.h"        // Task logic
#include <array>         // Arrays

#include "ssd1309.hpp"   // OLED Controller Driver
#include "zlib_turbo.h"  // ZLib Turbo
#include "spi.h"         // SSD1309 communicates via SPI 

#include "adc.h"         // ADC for voltage monitoring 

#include "i2s.h"         // I2S for audio playback

#include "graphics.hpp"  // Graphics controller
#include "sprites.hpp"   // Sprites controller
#include "tetromino.hpp" // Tetromino controller
#include "playfield.hpp" // Playfield controller
#include "audio.hpp"     // Audio controller
#include "melody.hpp"    // Melody controller
#include "decoder.hpp"   // Decoder controller
#include "video.hpp"     // Video controller

#include "types.hpp"

namespace {   

    /////////////////////////////////////////////////////////////////////////
    // Render Thread
    /////////////////////////////////////////////////////////////////////////

    std::array<uint8_t, SSD1309Driver::kFrameBufferSize> FrameBuffer{};
    SemaphoreHandle_t xFrameBufferMutex = NULL;
    StaticSemaphore_t xFrameBufferMutexBuffer;

    SemaphoreHandle_t xDisplayControlMutex = NULL;
    StaticSemaphore_t xDisplayControlMutexBuffer;

    SSD1309Driver oled { &hspi1, &FrameBuffer,
                            { OLED_CS_GPIO_Port,    OLED_CS_Pin },
                            { OLED_DC_GPIO_Port,    OLED_DC_Pin },
                            { OLED_RESET_GPIO_Port, OLED_RESET_Pin }};
    
    constexpr uint32_t    kRenderStackWords = 512;                  // stack depth in words (512 × 4 bytes = 2 KB)
    StackType_t           renderStack[kRenderStackWords];           // the task's own stack (32-bit words on Cortex-M)
    StaticTask_t          renderTcb;                                // FreeRTOS task control block for the task
    constexpr UBaseType_t kRenderPriority = tskIDLE_PRIORITY + 2;   // 2 levels above idle (higher = more important)
    TaskHandle_t          xRenderTaskHandle = NULL;                 // Task handle for notifications

    void renderTask(void*)
    {
        const bool oledInitOk = oled.init();
        configASSERT(oledInitOk);

        for (;;) {
            uint32_t ulInterruptValue = 0;
            xTaskNotifyWait(0x0, 0xFFFFFFFFUL, &ulInterruptValue, portMAX_DELAY);
            // Wait for render commands
            if (ulInterruptValue == CMD_BEGIN_RENDER)
            {
                if (xSemaphoreTake(xFrameBufferMutex, 0))
                {
                    bool result = oled.flush();
                    xSemaphoreGive(xFrameBufferMutex);
                    configASSERT(result);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////
    // Battery Thread
    /////////////////////////////////////////////////////////////////////////

    SemaphoreHandle_t xBatteryAdcMutex = NULL;
    StaticSemaphore_t xBatteryAdcMutexBuffer;

    typedef struct {
        uint32_t voltage;
        uint_fast8_t percentage; 
    } BatteryInfo_t;

    BatteryInfo_t batteryInfo {0, 0};

    SemaphoreHandle_t xBatteryInfoMutex = NULL;
    StaticSemaphore_t xBatteryInfoMutexBuffer;

    // Battery voltages to indexed "perdec", taken from battery.h
    constexpr uint32_t battery_mv_percentages[11] = {
        3305, 3686, 3741, 3775, 3793, 3820,
        3884, 3945, 4008, 4085, 4177
    };

    constexpr uint32_t    kBatteryLvlStackWords = 256;                  // stack depth in words (256 × 4 bytes = 1 KB)
    StackType_t           batteryLvlStack[kBatteryLvlStackWords];           // the task's own stack (32-bit words on Cortex-M)
    StaticTask_t          batteryLvlTcb;                                // FreeRTOS task control block for the task
    constexpr UBaseType_t kBattertLvlPriority = tskIDLE_PRIORITY;       // 0 levels above idle (higher = more important)

    void batteryLvlTask(void *)
    {
        for (;;) {
            uint32_t readVoltage = 0;
            uint32_t computedVoltage = 0;
            uint_fast8_t computedPercentage = 0;

            uint_fast8_t successfulReadings = 0;

            BatteryInfo_t oldInfo {0, 0};

            // Retrieve old info
            if (xSemaphoreTake(xBatteryInfoMutex, pdMS_TO_TICKS(16)))
            {
                oldInfo = batteryInfo;

                xSemaphoreGive(xBatteryInfoMutex);
            }

            // Attempt 10 readings from ADC
            // (but only if we don't already have a reading)
            uint8_t readAttempts = oldInfo.voltage > 0 ? 10 : 1;
            uint16_t sleepTime = oldInfo.voltage > 0 ? 750 : 10;
            for (uint_fast8_t i = 0; successfulReadings < readAttempts && i < 20; i++)
            {
                uint32_t temporaryVoltage = 0;

                if (xSemaphoreTake(xBatteryAdcMutex, pdMS_TO_TICKS(16)))
                {
                    if (HAL_ADC_Start(&hadc1) == HAL_OK)
                    {
                        if (HAL_ADC_PollForConversion(&hadc1, 5000) == HAL_OK)
                        {
                            temporaryVoltage = HAL_ADC_GetValue(&hadc1);
    
                            HAL_ADC_Stop(&hadc1);
                        }
                    }
    
                    xSemaphoreGive(xBatteryAdcMutex);
                }

                if (temporaryVoltage > 0)
                {
                    successfulReadings++;
                    readVoltage += temporaryVoltage;
                }

                vTaskDelay(pdMS_TO_TICKS(sleepTime));
            }

            // ONLY PROCEED IF WE HAVE HAD A SUCCESSFUL READING!
            if (successfulReadings > 0)
            {
                // Average out read voltage
                readVoltage /= successfulReadings;
    
                // Compute actual voltage as mV (readVoltage / 4.095 * 3.3 * 2)
                computedVoltage = readVoltage * 1.61172161;
    
                // Calculate percentage
                int32_t lower_bound_idx = -1; int64_t lower_bound = 0;
                int32_t upper_bound_idx = -1; int64_t upper_bound = 0;
    
                // Get lower and upper bounds
                for (size_t i = 0; i <= 10; i++)
                {
                    if (battery_mv_percentages[i] < computedVoltage)
                    {
                        lower_bound_idx = i;
                        lower_bound = battery_mv_percentages[i];
                    }
                    else
                    {
                        upper_bound_idx = i;
                        upper_bound = battery_mv_percentages[i];
                        break;
                    }
                }

                // Edge cases: 
                // new_mv < 0% will have lower_bound = -1 because no lower_bound exists
                if (lower_bound_idx == -1)
                {
                    computedPercentage = 0;
                }
                // new_mv > 100% will have upper_bound = -1 because no upper_bound exists
                else if (upper_bound_idx == -1)
                {
                    computedPercentage = 100;
                }
                // if we have lower AND upper bound, find percentage using geometric distance
                else
                {
                    int64_t bounds = upper_bound - lower_bound;
                    int64_t distance = (computedVoltage - lower_bound) * 10;
    
                    computedPercentage = lower_bound_idx * 10 + (distance/bounds);
                }
    
                // If we had previous readings, let's average them out with our new ones
                if (oldInfo.voltage > 0)
                    computedVoltage = (computedVoltage + oldInfo.voltage) / 2;

                if (oldInfo.percentage > 0)
                    computedPercentage = (computedPercentage + oldInfo.percentage) / 2;

                // Update battery info
                if (xSemaphoreTake(xBatteryInfoMutex, pdMS_TO_TICKS(16)))
                {
                    BatteryInfo_t newInfo = {computedVoltage, computedPercentage};
                    batteryInfo = newInfo;
    
                    xSemaphoreGive(xBatteryInfoMutex);
                }
            }

            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }

    
    /////////////////////////////////////////////////////////////////////////
    // Audio Thread
    /////////////////////////////////////////////////////////////////////////
    
    constexpr uint32_t    kAudioStackWords = 512;                  // stack depth in words (512 × 4 bytes = 2 KB)
    StackType_t           audioStack[kAudioStackWords];            // the task's own stack (32-bit words on Cortex-M)
    StaticTask_t          audioTcb;                                // FreeRTOS task control block for the task
    constexpr UBaseType_t kAudioPriority = tskIDLE_PRIORITY + 3;   // 3 levels above idle (higher = more important)
    QueueHandle_t         xAudioQueue = NULL;                      // Notes queue to avoid race conditions
    TaskHandle_t          xAudioTaskHandle = NULL;                 // Task handle for notifications



    // With the way we process the DMA, we can get away without a semaphore
    // in favor of the audio commands queue!

    volatile bool audioDmaFailed = false; // written in ISR, read in task

    static constexpr std::array<uint16_t, 12> noteToFrequency = {
        4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902
    };

    void audioTask(void *)
    {
        // this buffer stores 256 audio frames of "interweaved" stereo, which in 48kHz defaults to ~2.6ms (128/48)
        // with dma, we *NEED* to refill things in at !MOST! 1.3ms
        std::array<int16_t, 256> audioDMABuffer = {0};

        // Initialize audio controller
        AudioController audioController { &audioDMABuffer };

        // Initialize DMA protocol
        HAL_StatusTypeDef audioDMAStatus = HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t*)(audioDMABuffer.data()), 256);
        configASSERT(audioDMAStatus == HAL_OK);

        HAL_GPIO_WritePin(AUDIO_SHUTDOWN_GPIO_Port, AUDIO_SHUTDOWN_Pin, GPIO_PIN_SET);

        Command_t xReceivedCommand;

        uint32_t ulInterruptStatus = 0;
        uint32_t missedDeadlines = 0;

        for (;;)
        {
            xTaskNotifyWait(0x0, 0xFFFFFFFFL, &ulInterruptStatus, portMAX_DELAY);

            if ((ulInterruptStatus & 0x1) == 0x1) /* Dma error */
                {}
            if ((ulInterruptStatus & 0x2) == 0x2) /* Half Complete */
                audioController.processAudioCallback(false);
            if ((ulInterruptStatus & 0x4) == 0x4) /* Fully complete */
                audioController.processAudioCallback(true);
            if ((ulInterruptStatus & 0x6) == 0x6) /* Both half and full, we missed a deadline */
                missedDeadlines++;

            // Process any incoming notes
            while (xQueueReceive(xAudioQueue, &xReceivedCommand, 0) == pdTRUE)
            {
                if (xReceivedCommand.cmdType == CMD_AUDIO_PLAY_SOUND)
                {
                    uint32_t noteInfo = xReceivedCommand.cmdInfo;
                    uint16_t note = 0x1F & noteInfo;
    
                    if (note > 11)
                        continue;
    
                    uint16_t octave = (0xE0 & noteInfo) >> 5;
    
                    uint16_t frequency = noteToFrequency[note] >> (8 - octave);
    
                    audioController.playWave(
                        (0x78000000 & noteInfo) >> 27,
                        frequency,
                        (0xFFF00 & noteInfo) >> 8,
                        (0x7F00000 & noteInfo) >> 20
                    );
                }
            }
        }
    }

    extern "C" void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)
    {
        if (hi2s == &hi2s2 && xAudioTaskHandle != NULL)
        {
            audioDmaFailed = true;

            BaseType_t higherPriorityTaskWoken = pdFALSE;
            //configASSERT( == pdPASS);
            xTaskNotifyFromISR(xAudioTaskHandle, 0x1, eSetBits, &higherPriorityTaskWoken);
            portYIELD_FROM_ISR(higherPriorityTaskWoken);
        }
    }

    extern "C" void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
    {
        if (hi2s == &hi2s2 && xAudioTaskHandle != NULL)
        {
            BaseType_t higherPriorityTaskWoken = pdFALSE;
            xTaskNotifyFromISR(xAudioTaskHandle, 0x2, eSetBits, &higherPriorityTaskWoken);
            //configASSERT( == pdPASS);
            portYIELD_FROM_ISR(higherPriorityTaskWoken);
        }
    }

    extern "C" void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
    {
        if (hi2s == &hi2s2 && xAudioTaskHandle != NULL)
        {
            BaseType_t higherPriorityTaskWoken = pdFALSE;
            xTaskNotifyFromISR(xAudioTaskHandle, 0x4, eSetBits, &higherPriorityTaskWoken);
            //configASSERT( == pdPASS);
            portYIELD_FROM_ISR(higherPriorityTaskWoken);
        }
    }

    /////////////////////////////////////////////////////////////////////////
    // Melody Thread
    /////////////////////////////////////////////////////////////////////////
    
    constexpr uint32_t    kMelodyStackWords = 512;                  // stack depth in words (512 × 4 bytes = 2 KB)
    StackType_t           melodyStack[kMelodyStackWords];            // the task's own stack (32-bit words on Cortex-M)
    StaticTask_t          melodyTcb;                                // FreeRTOS task control block for the task
    constexpr UBaseType_t kMelodyPriority = tskIDLE_PRIORITY + 3;   // 3 levels above idle (higher = more important)
    TaskHandle_t          xMelodyTaskHandle = NULL;                 // Task handle for notifications

    void melodyTask(void *)
    {
        // Commands are copied instead of passed by reference, so we'll have this base command to help the code
        Command_t noteToPlay = {CMD_AUDIO_PLAY_SOUND, 0, nullptr};

        MelodyController melodyController;

        uint32_t receivedSongMsg = 0;
        uint32_t ulInterruptStatus = 0;

        bool receivedNotif = false;

        TickType_t timeToSleep = pdMS_TO_TICKS(0);
        TickType_t timeLastAwake = xTaskGetTickCount();

        for(;;)
        {
            receivedSongMsg = melodyController.getSongMessage();

            if ((receivedSongMsg & 0x80000000) == 0x80000000)
            {
                const uint32_t msToSleep = receivedSongMsg ^ 0x80000000;
                timeToSleep = pdMS_TO_TICKS(msToSleep);
                // Sleep while still staying awake for notifications
                receivedNotif = (xTaskNotifyWait(0x0, 0xFFFFFFFFL, &ulInterruptStatus, timeToSleep) == pdPASS);
            } else if (receivedSongMsg != 0)
            {
                noteToPlay.cmdInfo = receivedSongMsg;
                timeToSleep = pdMS_TO_TICKS(0);
                xQueueSend(xAudioQueue, (void *)&noteToPlay, 0);
            } else
            {
                // Sleep until notified that we should wake up
                receivedNotif = (xTaskNotifyWait(0x0, 0xFFFFFFFFL, &ulInterruptStatus, portMAX_DELAY) == pdPASS);
                timeLastAwake = xTaskGetTickCount();
                timeToSleep = pdMS_TO_TICKS(0);
            }

            if (receivedNotif)
            {
                if ((ulInterruptStatus & 0x80000000UL) != 0)
                {
                    // Special commands
                    switch (ulInterruptStatus)
                    {
                        case 0x80000001UL: // pause
                            receivedNotif = (xTaskNotifyWait(0x0, 0xFFFFFFFFL, &ulInterruptStatus, portMAX_DELAY) == pdPASS);
                            // resume should be discarded
                            if (ulInterruptStatus == 0x80000002)
                                receivedNotif = false;
                            break;

                        case 0x80000003UL: // stop
                            melodyController.playSong(0, false);
                            receivedNotif = false;
                            break;

                        default:
                            receivedNotif = false;
                            break;
                    }
                } else
                {
                    // Start new song
                    melodyController.playSong(ulInterruptStatus, true);
                    receivedNotif = false;
                }
            }

            if (timeToSleep > pdMS_TO_TICKS(0))
            {
                vTaskDelayUntil(&timeLastAwake, timeToSleep);
                timeLastAwake = xTaskGetTickCount();
            }
        }
    }


    /////////////////////////////////////////////////////////////////////////
    // Decoder Thread
    /////////////////////////////////////////////////////////////////////////
    
    constexpr uint32_t    kDecoderStackWords = 512;                  // stack depth in words (512 × 4 bytes = 2 KB)
    StackType_t           decoderStack[kDecoderStackWords];            // the task's own stack (32-bit words on Cortex-M)
    StaticTask_t          decoderTcb;                                // FreeRTOS task control block for the task
    constexpr UBaseType_t kDecoderPriority = tskIDLE_PRIORITY + 3;   // 3 levels above idle (higher = more important)
    TaskHandle_t          xDecoderTaskHandle = NULL;                 // Task handle for notifications
    TaskHandle_t          xVideoTaskHandle = NULL;                 // Task handle for notifications

    std::array<uint8_t, 36872> inflateBufferA{}; // Stores a block of 36 frames (86x64) + 8 bytes (per zlib turbo recommendation)
    std::array<uint8_t, 36872> inflateBufferB{}; // Stores a block of 36 frames (86x64) + 8 bytes (per zlib turbo recommendation)
    zlib_turbo zlibDecoder;

    void decoderTask(void *)
    {
        DecoderController decoderController { &zlibDecoder, &inflateBufferA, &inflateBufferB };

        decoderController.initializeDecoder(0);

        uint32_t ulInterruptStatus = 0;

        // Decode a block straight ahead
        decoderController.decodeBlock(true);

        xTaskNotify(xVideoTaskHandle, 0x1, eSetValueWithOverwrite);

        for(;;)
        {
            xTaskNotifyWait(0x0, 0xFFFFFFFFL, &ulInterruptStatus, portMAX_DELAY);
            switch (ulInterruptStatus)
            {
                case 0x1: // Video is checking buffer A
                    decoderController.decodeBlock(false);
                    break;

                case 0x2: // Video is checking buffer B
                    decoderController.decodeBlock(true);
                    break;

                default:
                    break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////
    // Video Thread
    /////////////////////////////////////////////////////////////////////////
    
    constexpr uint32_t    kVideoStackWords = 512;                  // stack depth in words (512 × 4 bytes = 2 KB)
    StackType_t           videoStack[kVideoStackWords];            // the task's own stack (32-bit words on Cortex-M)
    StaticTask_t          videoTcb;                                // FreeRTOS task control block for the task
    constexpr UBaseType_t kVideoPriority = tskIDLE_PRIORITY + 3;   // 3 levels above idle (higher = more important)

    void videoTask(void *)
    {
        VideoController videoController { &FrameBuffer, &xFrameBufferMutex };
        GraphicsController graphicsController {&FrameBuffer, &xFrameBufferMutex};
        SpritesController spritesController {&graphicsController};

        uint32_t ulInterruptStatus = 0;

        uint32_t videoFrame = 0;
        uint16_t videoBlockFrame = 0;

        const TickType_t xFrameTargetTimeOffset33 = pdMS_TO_TICKS(33);
        const TickType_t xFrameTargetTimeOffset34 = pdMS_TO_TICKS(34);

        std::array<uint8_t, 36872>* decodedBuffer = &inflateBufferA;

        bool checkingBufferA = true;

        // Wait until we have at least one block
        xTaskNotifyWait(0x0, 0xFFFFFFFFUL, &ulInterruptStatus, portMAX_DELAY);

        // Store battery info locally (to avoid not being able to access to it in case BatteryLvlTask is occupied)
        BatteryInfo_t batteryInfo_videoTask = {0, 0};

        TickType_t xLastFrameTime = xTaskGetTickCount();
        bool weHaveMutex = false;

        bool showMenuText = true;

        for(;;)
        {
            if(weHaveMutex)
            {
                xTaskNotify(xDecoderTaskHandle, checkingBufferA ? 0x1 : 0x2, eSetValueWithOverwrite);
                
                bool framesExist = true;
                while (framesExist)
                {
                    // Insert video frame
                    framesExist = videoController.displayVideoFrame(decodedBuffer, videoBlockFrame++);
                    
                    videoFrame++;

                    // Show menu text if wanted
                    if (showMenuText)
                    {
                        // Request battery level
                        if (xSemaphoreTake(xBatteryInfoMutex, pdMS_TO_TICKS(1)))
                        {
                            batteryInfo_videoTask = batteryInfo;

                            xSemaphoreGive(xBatteryInfoMutex);
                        }

                        // Non-linear, i know
                        if (batteryInfo_videoTask.percentage > 70)
                        {
                            spritesController.drawSprite(107, 121, 1);
                        }
                        else if (batteryInfo_videoTask.percentage > 50)
                        {
                            spritesController.drawSprite(108, 121, 1);
                        }
                        else if (batteryInfo_videoTask.percentage > 30)
                        {
                            spritesController.drawSprite(109, 121, 1);
                        }
                        else if (batteryInfo_videoTask.percentage > 5)
                        {
                            spritesController.drawSprite(110, 121, 1);
                        }
                        else
                        {
                            if (videoFrame % 60 >= 30)
                                spritesController.drawSprite(111, 121, 1);
                            else
                                spritesController.drawSprite(112, 121, 1);
                        }

                        if (videoFrame % 60 < 30)
                        {
                            spritesController.drawText(30, 7, "Press START");
                            spritesController.drawText(22, 19, "to play");
                        }
                    }
                    
                    // Send render command
                    xTaskNotify(xRenderTaskHandle, CMD_BEGIN_RENDER, eSetValueWithOverwrite);
                    
                    // This seeks to achieve exactly 30fps
                    if ((videoFrame & 0b11) == 0b11) // faster x % 3 == 0
                        vTaskDelayUntil(&xLastFrameTime, xFrameTargetTimeOffset33);
                    else
                        vTaskDelayUntil(&xLastFrameTime, xFrameTargetTimeOffset34);
                    xLastFrameTime = xTaskGetTickCount();

                    if (xTaskNotifyWait(0x0, 0xFFFFFFFFUL, &ulInterruptStatus, 0) == pdPASS)
                    {
                        if (ulInterruptStatus == CMD_SWITCH_DISPLAY_OWNERSHIP)
                        {
                            xSemaphoreGive(xDisplayControlMutex);
                            weHaveMutex = false;
                            vTaskDelayUntil(&xLastFrameTime, xFrameTargetTimeOffset33);
                            break;
                        }
                    }
                }
                
                // Swap buffers
                decodedBuffer = checkingBufferA ? &inflateBufferB : &inflateBufferA;
                checkingBufferA = !checkingBufferA;
                videoBlockFrame = 0;
            } else
            {
                xSemaphoreTake(xDisplayControlMutex, portMAX_DELAY);
                weHaveMutex = true;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////
    // Game Thread
    /////////////////////////////////////////////////////////////////////////
    
    constexpr uint32_t    kGameStackWords = 512;                  // stack depth in words (512 × 4 bytes = 2 KB)
    StackType_t           gameStack[kGameStackWords];             // the task's own stack (32-bit words on Cortex-M)
    StaticTask_t          gameTcb;                                // FreeRTOS task control block for the task
    constexpr UBaseType_t kGamePriority = tskIDLE_PRIORITY + 1;   // 1 level above idle (higher = more important)
    QueueHandle_t xGameInputQueue = NULL;                         // Input queue

    typedef enum
    {
        SCREEN_MAINMENU,
        SCREEN_GAME,
        SCREEN_LOW_BATTERY,
        SCREEN_SHUTDOWNMSG,
        SCREEN_ERR
    } Screen_t;

    Screen_t ScreenState = SCREEN_MAINMENU;

    void gameTask(void *)
    {

        // Salt for pseudo RNG
        uint32_t salt = 0;

        // Initialize all the controllers
        GraphicsController graphicsController {&FrameBuffer, &xFrameBufferMutex};
        SpritesController spritesController {&graphicsController};
        TetrominoController tetrominoController {&spritesController};
        PlayfieldController playfieldController {&tetrominoController, &spritesController, &graphicsController, &salt, xAudioQueue};

        // Get initial start time and set target time
        TickType_t xFrameStartTime = 0;
        TickType_t xPreviousFrameTime = 0;
        const TickType_t xFrameTargetTimeOffset33 = pdMS_TO_TICKS(33);
        const TickType_t xFrameTargetTimeOffset34 = pdMS_TO_TICKS(34);
        uint8_t updateMod3 = 0;

        bool weHaveDisplayControl = false;

        for (;;) {
            // Delta time calculations (okay to put here bc the last thing we do is 'delay until')
            xPreviousFrameTime = xFrameStartTime;
            xFrameStartTime = xTaskGetTickCount();
            TickType_t deltaTime = xFrameStartTime - xPreviousFrameTime;
            
            // Get paused state
            bool isGamePaused = playfieldController.getPaused();

            // Process input messages
            Command_t xReceivedCmd;
            Command_t noteToPlay = {CMD_AUDIO_PLAY_SOUND, 0, nullptr};
            while (xQueueReceive(xGameInputQueue, &xReceivedCmd, pdMS_TO_TICKS(1)) == pdTRUE)
            {
                salt += 26489;
                if (salt >= 2147457158)
                    salt -= 2147457158;

                switch (ScreenState)
                {
                    case SCREEN_GAME:
                    {
                        if (xReceivedCmd.cmdInfo == 1) // Pressing
                        {
                            switch (xReceivedCmd.cmdType)
                            {
                                case CMD_RIGHT_BTN:   playfieldController.moveCurrentPiece(true);    break;
                                case CMD_LEFT_BTN:    playfieldController.moveCurrentPiece(false);   break;
                                case CMD_ROT_CCW_BTN: playfieldController.rotateCurrentPiece(false); break;
                                case CMD_ROT_CW_BTN:  playfieldController.rotateCurrentPiece(true);  break;
                                case CMD_DOWN_BTN:    playfieldController.setSoftDropStatus(true);   break;
                                case CMD_UP_BTN:      playfieldController.hardDropCurrentPiece();    break;
                                case CMD_START_BTN:
                                    playfieldController.flipPaused();
                                    isGamePaused = playfieldController.getPaused();
                                    if (isGamePaused)
                                    {
                                        xTaskNotify(xMelodyTaskHandle, 0x80000001, eSetValueWithOverwrite);
                                        noteToPlay.cmdInfo = AudioController::sampleToUlValue(4, 0, 6, 20, 128);
                                        xQueueSend(xAudioQueue, (void *)&noteToPlay, 0);
                                        vTaskDelay(pdMS_TO_TICKS(128));
                                        noteToPlay.cmdInfo = AudioController::sampleToUlValue(4, 3, 6, 20, 128);
                                        xQueueSend(xAudioQueue, (void *)&noteToPlay, 0);
                                    }
                                    else
                                    {
                                        xTaskNotify(xMelodyTaskHandle, 0x80000002, eSetValueWithOverwrite);
                                        noteToPlay.cmdInfo = AudioController::sampleToUlValue(4, 3, 6, 20, 128);
                                        xQueueSend(xAudioQueue, (void *)&noteToPlay, 0);
                                        vTaskDelay(pdMS_TO_TICKS(128));
                                        noteToPlay.cmdInfo = AudioController::sampleToUlValue(4, 0, 6, 20, 128);
                                        xQueueSend(xAudioQueue, (void *)&noteToPlay, 0);
                                    }
                                    break;
                                case CMD_POWER_BTN:
                                    if (weHaveDisplayControl)
                                    {
                                        weHaveDisplayControl = false;
                                        xSemaphoreGive(xDisplayControlMutex);
                                        vTaskDelay(pdMS_TO_TICKS(1));
                                        ScreenState = SCREEN_MAINMENU;
                                        xTaskNotify(xMelodyTaskHandle, 0x80000003, eSetValueWithOverwrite);
                                        break;
                                    }
                                default:                                                             break;
                            }
                        }
                        else
                        {
                            switch (xReceivedCmd.cmdType) // Releasing
                            {
                                case CMD_DOWN_BTN:    playfieldController.setSoftDropStatus(false);   break;
                                default:                                                             break;
                            }
                        }
                    } break;

                    case SCREEN_MAINMENU:
                        if (xReceivedCmd.cmdInfo == 0) // Releasing
                        {
                            switch (xReceivedCmd.cmdType)
                            {
                                // Open game when releasing pause
                                case CMD_START_BTN:   {
                                    ScreenState = SCREEN_GAME; 
                                    xTaskNotify(xMelodyTaskHandle, 0x1, eSetValueWithOverwrite);
                                    playfieldController.initializePlayfield();  
                                    while (!weHaveDisplayControl)
                                    {
                                        xTaskNotify(xVideoTaskHandle, CMD_SWITCH_DISPLAY_OWNERSHIP, eSetValueWithOverwrite);
                                        if (xSemaphoreTake(xDisplayControlMutex, pdMS_TO_TICKS(1)) == pdTRUE)
                                        {
                                            weHaveDisplayControl = true;
                                        }
                                    }
                                } break;
                                case CMD_ROT_CCW_BTN: 
                                    noteToPlay.cmdInfo = AudioController::sampleToUlValue(4, 0, 6, 20, 250);
                                    xQueueSend(xAudioQueue, (void *)&noteToPlay, 0);
                                    break;
                                case CMD_ROT_CW_BTN:  
                                    noteToPlay.cmdInfo = AudioController::sampleToUlValue(4, 2, 6, 20, 250);
                                    xQueueSend(xAudioQueue, (void *)&noteToPlay, 0);
                                    break;
                                case CMD_UP_BTN:      
                                    noteToPlay.cmdInfo = AudioController::sampleToUlValue(4, 4, 6, 20, 250);
                                    xQueueSend(xAudioQueue, (void *)&noteToPlay, 0);
                                    break;
                                case CMD_DOWN_BTN:    
                                    noteToPlay.cmdInfo = AudioController::sampleToUlValue(4, 5, 6, 20, 250);
                                    xQueueSend(xAudioQueue, (void *)&noteToPlay, 0);
                                    break;
                                case CMD_LEFT_BTN:    
                                    noteToPlay.cmdInfo = AudioController::sampleToUlValue(4, 7, 6, 20, 250);
                                    xQueueSend(xAudioQueue, (void *)&noteToPlay, 0);
                                    break;
                                case CMD_RIGHT_BTN:   
                                    noteToPlay.cmdInfo = AudioController::sampleToUlValue(4, 9, 6, 20, 250);
                                    xQueueSend(xAudioQueue, (void *)&noteToPlay, 0);
                                    break;
                                default: break;
                            }
                            break;
                        }

                    default: break;
                }
            }

            // Per-screen behaviours
            switch (ScreenState)
            {
                case SCREEN_GAME:
                {
                    playfieldController.tickPlayfield(deltaTime, xFrameStartTime);
                    if (weHaveDisplayControl)
                    {   
                        // Clear framebuffer
                        if (xSemaphoreTake(xFrameBufferMutex, pdMS_TO_TICKS(16)))
                        {
                            oled.clear();
                            xSemaphoreGive(xFrameBufferMutex);
                        }
                        playfieldController.renderPlayfield();
                        playfieldController.renderPlayfieldHeader();
                    }
                    break;
                }

                default:
                    break;
            }


            // Send render command
            if (weHaveDisplayControl)
            {
                xTaskNotify(xRenderTaskHandle, CMD_BEGIN_RENDER, eSetValueWithOverwrite);
            }

            // This seeks to achieve exactly 30fps
            if (updateMod3++ != 2)
            vTaskDelayUntil(&xFrameStartTime, xFrameTargetTimeOffset33);
            else {
                vTaskDelayUntil(&xFrameStartTime, xFrameTargetTimeOffset34);
                updateMod3 = 0;
            }
        }
    }
    
    /////////////////////////////////////////////////////////////////////////
    // Button ISR Callback
    /////////////////////////////////////////////////////////////////////////

    extern "C" void HAL_GPIO_EXTI_Callback(uint16_t gpioPin)
    {
        // ignore presses before queue is ready
        if (xGameInputQueue == nullptr)
            return;
        
        // Although we only receive the gpio pin number, since we connected them
        // to different numbers, we can get away with using the number to find
        // the port, since we never have PCx and PBx for the same x!
        
        // THIS ISN'T AN EXPRESS GUARANTEE IN OTHER CIRCUITS, but it is the case in this one mwehehehe
        
        // Since buttons are arranged as PC2, PC5, PC6, PC7, PB8, PB11, PB13 and PB14 at time of writing,
        // we can get away with a "is this pin larger or equal to 8?" to swap port C for B!
        
        GPIO_TypeDef *port = (gpioPin >= BTN_CW_Pin) ? GPIOB : GPIOC;
        GPIO_PinState state = HAL_GPIO_ReadPin(port, gpioPin);
        
        uint32_t info = (state != GPIO_PIN_SET) ? 1 : 0;
        
        Command_t inputCmd = {CMD_NO_OP, info, nullptr};
        
        switch (gpioPin)
        {
            case BTN_CCW_Pin:   inputCmd.cmdType = CMD_ROT_CCW_BTN; break;
            case BTN_CW_Pin:    inputCmd.cmdType = CMD_ROT_CW_BTN;  break;
            case BTN_LEFT_Pin:  inputCmd.cmdType = CMD_LEFT_BTN;    break;
            case BTN_RIGHT_Pin: inputCmd.cmdType = CMD_RIGHT_BTN;   break;
            case BTN_UP_Pin:    inputCmd.cmdType = CMD_UP_BTN;      break;
            case BTN_DOWN_Pin:  inputCmd.cmdType = CMD_DOWN_BTN;    break;
            case BTN_START_Pin: inputCmd.cmdType = CMD_START_BTN;   break;
            case BTN_POWER_Pin: inputCmd.cmdType = CMD_POWER_BTN;   break;
        }
        
        BaseType_t higherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(xGameInputQueue, (void *)&inputCmd, &higherPriorityTaskWoken);
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
 
}

/////////////////////////////////////////////////////////////////////////
// Application Launch
/////////////////////////////////////////////////////////////////////////

extern "C" void app_init(void)
{
    // Initialize display mutexes
    xFrameBufferMutex = xSemaphoreCreateMutexStatic(&xFrameBufferMutexBuffer);
    xDisplayControlMutex = xSemaphoreCreateMutexStatic(&xDisplayControlMutexBuffer);

    // Initialize battery adc mutex
    xBatteryAdcMutex = xSemaphoreCreateMutexStatic(&xBatteryAdcMutexBuffer);

    // Initialize battery info mutex
    xBatteryInfoMutex = xSemaphoreCreateMutexStatic(&xBatteryInfoMutexBuffer);

    // Launch render task
    TaskHandle_t render = xTaskCreateStatic(renderTask, "render", kRenderStackWords,
                                       nullptr, kRenderPriority,
                                       renderStack, &renderTcb);
    configASSERT(render != nullptr);   // Confirm task was successfully created!
    xRenderTaskHandle = render;

    // Launch battery task
    TaskHandle_t battery = xTaskCreateStatic(batteryLvlTask, "battery", kBatteryLvlStackWords,
                                       nullptr, kBattertLvlPriority,
                                       batteryLvlStack, &batteryLvlTcb);
    configASSERT(battery != nullptr);   // Confirm task was successfully created!

    // Launch audio task
    TaskHandle_t audio = xTaskCreateStatic(audioTask, "audio", kAudioStackWords,
                                       nullptr, kAudioPriority,
                                       audioStack, &audioTcb);
    configASSERT(audio != nullptr);   // Confirm task was successfully created!
    xAudioQueue = xQueueCreate(32, sizeof(Command_t));
    configASSERT(xAudioQueue != nullptr);   // Confirm queue was successfully created!
    xAudioTaskHandle = audio;

    // Launch melody task
    TaskHandle_t melody = xTaskCreateStatic(melodyTask, "melody", kMelodyStackWords,
                                       nullptr, kMelodyPriority,
                                       melodyStack, &melodyTcb);
    configASSERT(melody != nullptr);   // Confirm task was successfully created!
    xMelodyTaskHandle = melody;


    // Launch decoder task
    TaskHandle_t decoder = xTaskCreateStatic(decoderTask, "decoder", kDecoderStackWords,
                                       nullptr, kDecoderPriority,
                                       decoderStack, &decoderTcb);
    configASSERT(decoder != nullptr);   // Confirm task was successfully created!
    xDecoderTaskHandle = decoder;

    // Launch video task
    TaskHandle_t video = xTaskCreateStatic(videoTask, "video", kVideoStackWords,
                                        nullptr, kVideoPriority,
                                        videoStack, &videoTcb);
    configASSERT(video != nullptr);
    xVideoTaskHandle = video;

    // Launch game task
    TaskHandle_t game = xTaskCreateStatic(gameTask, "game", kGameStackWords,
                                       nullptr, kGamePriority,
                                       gameStack, &gameTcb);
    configASSERT(game != nullptr);   // Confirm task was successfully created!
    xGameInputQueue = xQueueCreate(10, sizeof(Command_t));
    configASSERT(xGameInputQueue != nullptr);   // Confirm queue was successfully created!
}