#include "keyboard_map.h"

uint16_t keys[] = {
    KEY_ESC,        0x1e,     0x1f,     0x20,     0x21,     0x22,     0x23,     0x24,     0x25,     0x26,     0x27,     0x2d,      0x2e,            KEY_BACKSPACE, 0x35,
    KEY_TAB,        0x14,     0x1a,     0x08,     0x15,     0x17,     0x1c,     0x18,     0x0c,     0x12,     0x13,     0x2f,      0x30,            0x31,          KEY_DELETE,
    KEY_CAPS_LOCK,  0x04,     0x16,     0x07,     0x09,     0x0a,     0x0b,     0x0d,     0x0e,     0x0f,     0x33,     0x34,      0x00,            KEY_RETURN,    KEY_PAGE_UP,
    KEY_LEFT_SHIFT, 0x1d,     0x1b,     0x06,     0x19,     0x05,     0x11,     0x10,     0x36,     0x37,     0x38,     0x00,      KEY_RIGHT_SHIFT, KEY_UP_ARROW,  KEY_PAGE_DOWN,
    KEY_LEFT_CTRL,  KEY_LEFT_GUI, KEY_LEFT_ALT, 0x00, 0x00, 0x2c,     0x00,     0x00,     0x00, KEY_RIGHT_ALT, 0x00, KEY_RIGHT_CTRL, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW,
};
 
uint16_t keys_alternate[] = { 
    0x00, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, 0x00,   0x46, 
    0x00, 0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,    0x00,    0x00, KEY_MEDIA_MUTE,   KEY_INSERT,
    0x00, 0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,    0x00,    0x00,    0x00,   KEY_HOME,
    0x00, 0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,    0x00,    0x00, KEY_MEDIA_VOLUP, KEY_END,
    0x00, 0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,   0x00,    0x00,    0x00, KEY_MEDIA_VOLDN, 0x00,
};

uint32_t USBD_Keyboard_press(KeyboardHID_t* pKeyboardHid, uint16_t k)
{
    uint8_t i;
    if ((k & 0xff00) == 0x100)
    { // it's a modifier key
        pKeyboardHid->MODIFIER |= k & 0xff;
        k = 0;
    }
    if (!k)
        return 0;

    for (i = 0; i < 6; i++)
    {
        if (pKeyboardHid->KEYCODE[i] == 0x00)
        {
            pKeyboardHid->KEYCODE[i] = k;
            return 1;
        }
    }
    return 0;
}
void USBD_Keyboard_releaseAll(KeyboardHID_t* pKeyboardHid)
{
    pKeyboardHid->KEYCODE[0] = 0;
    pKeyboardHid->KEYCODE[1] = 0;
    pKeyboardHid->KEYCODE[2] = 0;
    pKeyboardHid->KEYCODE[3] = 0;
    pKeyboardHid->KEYCODE[4] = 0;
    pKeyboardHid->KEYCODE[5] = 0;
    pKeyboardHid->MODIFIER = 0;
    // USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&pKeyboardHid, sizeof(pKeyboardHid));
}
