// this is the implementation file for keyboard.h

#include "keyboard.h"

static uint8_t keyboard_input_make_array[36] = {

    0x1C, 0x32, 0x21, 0x23, 0x24, 0x2B,     // A,B,C,D,E,F
    0x34, 0x33, 0x43, 0x3B, 0x42, 0x4B,     // G,H,I,J,K,L
    0x3A, 0x31, 0x44, 0x4D, 0x15, 0x2D,     // M,N,O,P,Q,R
    0x1B, 0x2C, 0x3C, 0x2A, 0x1D, 0x22,     // S,T,U,V,W,X
    0x35, 0x1A, 0x45, 0x16, 0x1E, 0x26,     // Y,Z,0,1,2,3
    0x25, 0x2E, 0x36, 0x3D, 0x3E, 0x46      // 4,5,6,7,8,9

};

static uint8_t ascii_val[36] = {

    'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l',
    'm', 'n', 'o', 'p', 'q', 'r',
    's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9'

};
void keyboard_init(void)
{
    /*
        1) we need to read from the keyboard port (x60)
        2) we need to enable the keyboard port on the PIC
    */

    //uint8_t keyboard_read;

    //keyboard_read = inb(KEYBOARD_BUFFER_PORT);
    enable_irq(KEYBOARD_IRQ_NUM);
}

void keyboard_handler()
{
    /*
        1) create the mapping mechanism
        2) call end of interrupt signal
    */

    // perform mapping mechanism
    uint8_t keyboard_read, i;

    keyboard_read = inb(KEYBOARD_BUFFER_PORT);

    for(i = 0; i < 36; i++)
    {
        if(keyboard_read == keyboard_input_make_array[i])
        {
            putc(ascii_val[i]);
            break;
        }
    }

    send_eoi(KEYBOARD_IRQ_NUM);
}
