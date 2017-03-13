// this is the implementation file for keyboard.h

#include "keyboard.h"

static uint8_t keyboard_input_make_array[36] = {

    0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21,     // A,B,C,D,E,F
    0x22, 0x23, 0x17, 0x24, 0x25, 0x26,     // G,H,I,J,K,L
    0x32, 0x31, 0x18, 0x19, 0x10, 0x13,     // M,N,O,P,Q,R
    0x1F, 0x14, 0x16, 0x2F, 0x11, 0x2D,     // S,T,U,V,W,X
    0x15, 0x2C, 0x0B, 0x02, 0x03, 0x04,     // Y,Z,0,1,2,3
    0x05, 0x06, 0x07, 0x08, 0x09, 0x0A      // 4,5,6,7,8,9

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
    //uint32_t flags;
    //cli_and_save(flags);
    enable_irq(KEYBOARD_IRQ_NUM);
    //restore_flags(flags);
}

void keyboard_handler()
{
    /*
        1) create the mapping mechanism
        2) call end of interrupt signal
    */


    // perform mapping mechanism
   uint8_t keyboard_read, i;
  //  int loopflag = 0;

    keyboard_read = inb(KEYBOARD_BUFFER_PORT);
    //printf("%d",keyboard_read);
    for(i = 0; i < 36; i++)
    {
        if(keyboard_read == keyboard_input_make_array[i])
        {
            putc(ascii_val[i]);
            //loopflag = 1;
            break;
        }
    }
    /*if(loopflag==1)
      break;*/

    send_eoi(KEYBOARD_IRQ_NUM);

    //iret();
}
