#pragma once
#include <stdint.h>
#include <windows.h>

enum CTRL_MSG_TYPE {
    MOUSE_EVENT_TYPE = 0,
    KEYBOARD_EVENT_TYPE
};

struct EventMsgBase {
    int32_t type;
    int32_t length;
    uint8_t version;
};

struct MouseEventMsg {
    EventMsgBase base;
    uint32_t mouse_event;
    uint16_t x;
    uint16_t y;
    int32_t extData;
};

struct KeyboardEventMsg {
    EventMsgBase base;
    uint32_t vkey;
    uint32_t state;
};

const char *getMouseEventName(int evt);

