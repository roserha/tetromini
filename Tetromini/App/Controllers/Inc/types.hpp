#pragma once
#include <cstdint>

/////////////////////////////////////////////////////////////////////////
// Inter-thread commands
/////////////////////////////////////////////////////////////////////////

typedef enum {
    CMD_NO_OP,
    CMD_BEGIN_RENDER,
    CMD_UP_BTN,
    CMD_DOWN_BTN,
    CMD_LEFT_BTN,
    CMD_RIGHT_BTN,
    CMD_ROT_CCW_BTN,
    CMD_ROT_CW_BTN,
    CMD_START_BTN,
    CMD_POWER_BTN,
    CMD_AUDIO_DMA_HALF_COMPLETE,
    CMD_AUDIO_DMA_COMPLETE,
    CMD_AUDIO_DMA_ERROR,
    CMD_AUDIO_PLAY_SOUND
} CommandType_t;

typedef struct {
    CommandType_t cmdType;
    uint32_t cmdInfo;
    void * cmdPayload;
} Command_t;