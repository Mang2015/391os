// this is the header file for initializing the keyboard

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"

#define KEYBOARD_BUFFER_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define KEYBOARD_IRQ_NUM 1

uint8_t keyboard_input_make_array[36] = {

    0x1C, 0x32, 0x21, 0x23, 0x24, 0x2B,     // A,B,C,D,E,F
    0x34, 0x33, 0x43, 0x3B, 0x42, 0x4B,     // G,H,I,J,K,L
    0x3A, 0x31, 0x44, 0x4D, 0x15, 0x2D,     // M,N,O,P,Q,R
    0x1B, 0x2C, 0x3C, 0x2A, 0x1D, 0x22,     // S,T,U,V,W,X
    0x35, 0x1A, 0x45, 0x16, 0x1E, 0x26,     // Y,Z,0,1,2,3
    0x25, 0x2E, 0x36, 0x3D, 0x3E, 0x46      // 4,5,6,7,8,9

};

uint8_t ascii_val[36] = {

    'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l',
    'm', 'n', 'o', 'p', 'q', 'r',
    's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9'
    
};

extern void keyboard_init(void);
extern void keyboard_handler();
