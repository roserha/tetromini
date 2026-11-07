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
    CMD_AUDIO_PLAY_SOUND,
    CMD_SWITCH_DISPLAY_OWNERSHIP,
    CMD_HIDE_MENU_INFO
} CommandType_t;

typedef struct {
    CommandType_t cmdType;
    uint32_t cmdInfo;
    void * cmdPayload;
} Command_t;

// Which tetromino is to be drawn
typedef enum {
    Empty = 0,
    TBlock = 1,
    OBlock = 2,
    IBlock = 3,
    ZBlock = 4,
    SBlock = 5,
    LBlock = 6,
    JBlock = 7
} Tetromino_t;

// How the tetromino is spun
// Defined per bit to make spinning math easier
// based on simplified direction vector * rotation matrix math
// 1s bit means flip x and ys
// 2s bit means negate direction
typedef enum{
    ZeroDeg = 0b00,     
    TwoSeventyDeg = 0b01,
    OneEightyDeg = 0b10,
    NinetyDeg = 0b11
} Spin_t;