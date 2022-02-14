#ifndef KEYBOARD_MAP_H
#define KEYBOARD_MAP_H

#include "usbd_hid.h"
#include "stdint.h"

#define KEY_LEFT_CTRL ((uint16_t)0x101)
#define KEY_LEFT_SHIFT ((uint16_t)0x102)
#define KEY_LEFT_ALT ((uint16_t)0x104)
#define KEY_LEFT_GUI ((uint16_t)0x108)
#define KEY_RIGHT_CTRL ((uint16_t)0x110)
#define KEY_RIGHT_SHIFT ((uint16_t)0x120)
#define KEY_RIGHT_ALT ((uint16_t)0x140)
#define KEY_RIGHT_GUI ((uint16_t)0x180)

#define KEY_UP_ARROW ((uint16_t)0x52)
#define KEY_DOWN_ARROW ((uint16_t)0x51)
#define KEY_LEFT_ARROW ((uint16_t)0x50)
#define KEY_RIGHT_ARROW ((uint16_t)0x4f)
#define KEY_BACKSPACE ((uint16_t)0x2a)
#define KEY_TAB ((uint16_t)0x2b)
#define KEY_RETURN ((uint16_t)0x28)
#define KEY_ESC ((uint16_t)0x29)
#define KEY_INSERT ((uint16_t)0x49)
#define KEY_PRTSC ((uint16_t)0x46)
#define KEY_DELETE ((uint16_t)0x4c)
#define KEY_PAGE_UP ((uint16_t)0x4b)
#define KEY_PAGE_DOWN ((uint16_t)0x4e)
#define KEY_HOME ((uint16_t)0x4a)
#define KEY_END ((uint16_t)0x4d)
#define KEY_CAPS_LOCK ((uint16_t)0x39)
#define KEY_F1 ((uint16_t)0x3a)
#define KEY_F2 ((uint16_t)0x3b)
#define KEY_F3 ((uint16_t)0x3c)
#define KEY_F4 ((uint16_t)0x3d)
#define KEY_F5 ((uint16_t)0x3e)
#define KEY_F6 ((uint16_t)0x3f)
#define KEY_F7 ((uint16_t)0x40)
#define KEY_F8 ((uint16_t)0x41)
#define KEY_F9 ((uint16_t)0x42)
#define KEY_F10 ((uint16_t)0x43)
#define KEY_F11 ((uint16_t)0x44)
#define KEY_F12 ((uint16_t)0x45)
#define KEY_F13 ((uint16_t)0x68)
#define KEY_F14 ((uint16_t)0x69)
#define KEY_F15 ((uint16_t)0x6a)
#define KEY_F16 ((uint16_t)0x6b)
#define KEY_F17 ((uint16_t)0x6c)
#define KEY_F18 ((uint16_t)0x6d)
#define KEY_F19 ((uint16_t)0x6e)
#define KEY_F20 ((uint16_t)0x6f)
#define KEY_F21 ((uint16_t)0x70)
#define KEY_F22 ((uint16_t)0x71)
#define KEY_F23 ((uint16_t)0x72)
#define KEY_F24 ((uint16_t)0x73)
#define KEY_NUM_0 ((uint16_t)0x62)
#define KEY_NUM_1 ((uint16_t)0x59)
#define KEY_NUM_2 ((uint16_t)0x5a)
#define KEY_NUM_3 ((uint16_t)0x5b)
#define KEY_NUM_4 ((uint16_t)0x5c)
#define KEY_NUM_5 ((uint16_t)0x5d)
#define KEY_NUM_6 ((uint16_t)0x5e)
#define KEY_NUM_7 ((uint16_t)0x5f)
#define KEY_NUM_8 ((uint16_t)0x60)
#define KEY_NUM_9 ((uint16_t)0x61)
#define KEY_NUM_SLASH ((uint16_t)0x54)
#define KEY_NUM_ASTERISK ((uint16_t)0x55)
#define KEY_NUM_MINUS ((uint16_t)0x56)
#define KEY_NUM_PLUS ((uint16_t)0x57)
#define KEY_NUM_ENTER ((uint16_t)0x58)
#define KEY_NUM_PERIOD ((uint16_t)0x63)

#define KEY_MEDIA_MUTE ((uint16_t)0x80e2)
#define KEY_MEDIA_VOLUP ((uint16_t)0x80e9)
#define KEY_MEDIA_VOLDN ((uint16_t)0x80ea)

typedef struct
{
    uint8_t ID;
    uint8_t MODIFIER;
    uint8_t RESERVED;
    uint8_t KEYCODE[6];
} KeyboardHID_t;

extern uint16_t keys[];
extern uint16_t keys_alternate[];


uint32_t USBD_Keyboard_press(KeyboardHID_t* pKeyboardHid, uint16_t k);
void USBD_Keyboard_releaseAll(KeyboardHID_t* pKeyboardHid);

#endif //KEYBOARD_MAP_H