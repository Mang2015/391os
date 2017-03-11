// this is the header file for initializing the keyboard

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

#define KEYBOARD_BUFFER_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_IRQ_NUM 1

extern void keyboard_init(void);

extern void keyboard_handler();
