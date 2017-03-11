// this is the implementation file for keyboard.h

#include "keyboard.h"

void keyboard_init(void)
{
    /*
        1) we need to read from the keyboard port (x60)
        2) we need to enable the keyboard port on the PIC
    */

    uint8_t keyboard_read;

    keyboard_read = inb(KEYBOARD_BUFFER_PORT);
    enable_irq(KEYBOARD_IRQ_NUM);
}

void keyboard_handler()
{
    /*
        1) create the mapping mechanism
        2) call end of interrupt signal
    */

    // perform mapping mechanism

    send_eoi(KEYBOARD_IRQ_NUM);
}
