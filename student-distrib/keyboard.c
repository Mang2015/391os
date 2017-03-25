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

static uint8_t ascii_val_upper[36] = {

    'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'I', 'J', 'K', 'L',
    'M', 'N', 'O', 'P', 'Q', 'R',
    'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9'

};

uint8_t line_char_buffer[128];
uint32_t capslock_flag;

uint8_t bksp_flag;
uint8_t buffIdx;



/* keyboard_init
 *
 * DESCRIPTION: Enables keyboard IRQ on PIC
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: none
 */
void keyboard_init(void)
{
    // enable the IRQ on PIC associated with keyboard
    enable_irq(KEYBOARD_IRQ_NUM);
    buffIdx = -1;
    capslock_flag = 0;
    bksp_flag = 0;
}

/* keyboard_handler
 *
 * DESCRIPTION: Handler called by IDT in response to a keyboard interrupt.
 *              Reads in make code of key from port, loops through an array to
 *              find the corresponding index for the appropriate Ascii value, and
 *              then puts the ascii value to screen.
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: Sends EOI to PIC(S)
 */
void keyboard_handler()
{
    /*
        1) create the mapping mechanism
        2) call end of interrupt signal
    */


    // perform mapping mechanism
    uint8_t keyboard_read;

    // take in the port value holding the make code for letter
    keyboard_read = inb(KEYBOARD_BUFFER_PORT);

    if (keyboard_read == CAPS) {
      if (capslock_flag != 1) {
        capslock_flag = 1;
      }
      else {
        capslock_flag = 0;
      }
    }                   // turn on/off caps lock
    else if (keyboard_read == BKSP)
      bksp_handler();
    else if ((keyboard_read == SCROLL_UP) || (keyboard_read == SCROLL_DOWN))
     scrollPage();
    else
      keyboardBuff(keyboard_read);

    // send end of interrupt signal so other interrupts can be processed
    send_eoi(KEYBOARD_IRQ_NUM);
}

void keyboardBuff(uint8_t keyboard_read) {
  //increase the buffer index to keep track of last place in buffer
  buffIdx++;
  int i;

  //return if buffer overflow
  if(buffIdx == 128)
    return;

  //add char to the buffer based on the scancode
  for(i = 0; i < 36; i++)
  {
      if(keyboard_read == keyboard_input_make_array[i])
      {
          if(capslock_flag){
            line_char_buffer[buffIdx] = ascii_val_upper[i];
            break;
          }
          else{
            line_char_buffer[buffIdx] = ascii_val[i];
            break;
          }

      }
  }

  output_buffer();
  return;

}

void bksp_handler() {



return;

}

void scrollPage() {


return;

}

void output_buffer() {

  int i;

  for(i = 0; i < buffIdx + 1; i++)
  {
    putc(line_char_buffer[buffIdx]);
  }

  return;

}
