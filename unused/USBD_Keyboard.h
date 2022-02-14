#ifndef USBD_KEYBOARD_H
#define USBD_KEYBOARD_H

#include "usbd_hid.h"

#define KEY_LEFT_CTRL ((unsigned char)0x80)
#define KEY_LEFT_SHIFT ((unsigned char)0x81)
#define KEY_LEFT_ALT ((unsigned char)0x82)
#define KEY_LEFT_GUI ((unsigned char)0x83)
#define KEY_RIGHT_CTRL ((unsigned char)0x84)
#define KEY_RIGHT_SHIFT ((unsigned char)0x85)
#define KEY_RIGHT_ALT ((unsigned char)0x86)
#define KEY_RIGHT_GUI ((unsigned char)0x87)

#define KEY_UP_ARROW ((unsigned char)0xDA)
#define KEY_DOWN_ARROW ((unsigned char)0xD9)
#define KEY_LEFT_ARROW ((unsigned char)0xD8)
#define KEY_RIGHT_ARROW ((unsigned char)0xD7)
#define KEY_BACKSPACE ((unsigned char)0xB2)
#define KEY_TAB ((unsigned char)0xB3)
#define KEY_RETURN ((unsigned char)0xB0)
#define KEY_ESC ((unsigned char)0xB1)
#define KEY_INSERT ((unsigned char)0xD1)
#define KEY_PRTSC ((unsigned char)0xCE)
#define KEY_DELETE ((unsigned char)0xD4)
#define KEY_PAGE_UP ((unsigned char)0xD3)
#define KEY_PAGE_DOWN ((unsigned char)0xD6)
#define KEY_HOME ((unsigned char)0xD2)
#define KEY_END ((unsigned char)0xD5)
#define KEY_CAPS_LOCK ((unsigned char)0xC1)
#define KEY_F1 ((unsigned char)0xC2)
#define KEY_F2 ((unsigned char)0xC3)
#define KEY_F3 ((unsigned char)0xC4)
#define KEY_F4 ((unsigned char)0xC5)
#define KEY_F5 ((unsigned char)0xC6)
#define KEY_F6 ((unsigned char)0xC7)
#define KEY_F7 ((unsigned char)0xC8)
#define KEY_F8 ((unsigned char)0xC9)
#define KEY_F9 ((unsigned char)0xCA)
#define KEY_F10 ((unsigned char)0xCB)
#define KEY_F11 ((unsigned char)0xCC)
#define KEY_F12 ((unsigned char)0xCD)
#define KEY_F13 ((unsigned char)0xF0)
#define KEY_F14 ((unsigned char)0xF1)
#define KEY_F15 ((unsigned char)0xF2)
#define KEY_F16 ((unsigned char)0xF3)
#define KEY_F17 ((unsigned char)0xF4)
#define KEY_F18 ((unsigned char)0xF5)
#define KEY_F19 ((unsigned char)0xF6)
#define KEY_F20 ((unsigned char)0xF7)
#define KEY_F21 ((unsigned char)0xF8)
#define KEY_F22 ((unsigned char)0xF9)
#define KEY_F23 ((unsigned char)0xFA)
#define KEY_F24 ((unsigned char)0xFB)

#define KEY_NUM_0 ((unsigned char)0xEA)
#define KEY_NUM_1 ((unsigned char)0xE1)
#define KEY_NUM_2 ((unsigned char)0xE2)
#define KEY_NUM_3 ((unsigned char)0xE3)
#define KEY_NUM_4 ((unsigned char)0xE4)
#define KEY_NUM_5 ((unsigned char)0xE5)
#define KEY_NUM_6 ((unsigned char)0xE6)
#define KEY_NUM_7 ((unsigned char)0xE7)
#define KEY_NUM_8 ((unsigned char)0xE8)
#define KEY_NUM_9 ((unsigned char)0xE9)
#define KEY_NUM_SLASH ((unsigned char)0xDC)
#define KEY_NUM_ASTERISK ((unsigned char)0xDD)
#define KEY_NUM_MINUS ((unsigned char)0xDE)
#define KEY_NUM_PLUS ((unsigned char)0xDF)
#define KEY_NUM_ENTER ((unsigned char)0xE0)
#define KEY_NUM_PERIOD ((unsigned char)0xEB)

typedef struct
{
    uint8_t MODIFIER;
    uint8_t RESERVED;
    uint8_t KEYCODE[6];
} KeyboardHID_t;

void USBD_Keyboard_releaseAll(void);
size_t USBD_Keyboard_release(uint8_t k);
size_t USBD_Keyboard_press(uint8_t k);
HID_StateTypeDef USBD_Keyboard_State();
void USBD_Keyboard_pressKeys(uint8_t* keys, size_t nKeys);
void USBD_Keyboard_releaseKeys(uint8_t* keys, size_t nKeys);
void USBD_Keyboard_writeString(uint8_t* keys, size_t nKeys);
void USBD_Keyboard_write(uint8_t key);

#endif// USBD_KEYBOARD_H