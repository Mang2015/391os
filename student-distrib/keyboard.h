// this is the header file for initializing the keyboard
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

#define KEYBOARD_BUFFER_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_IRQ_NUM 1
#define CAPS 0x3A
#define BKSP 0x0E
#define SCROLL_UP 0x48
#define SCROLL_DOWN 0x50
#define LSHIFT_PRESS 0x2A
#define LSHIFT_RELEASE 0xAA


extern void keyboard_init(void);
extern void keyboard_handler();
extern void bksp_handler();
extern void scrollPage();
extern void keyboardBuff(uint8_t keyboard_read);
extern void output_buffer();
extern void LRshift(uint8_t keyboard_read);

#endif
