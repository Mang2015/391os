#include "terminal.h"

/* terminal_open
 * input: none
 * output: none
 * side effects: none
 * function: function to open the terminal, returns 0 on success
 */
int32_t terminal_open(){
  return 0;
}

/* terminal_close
 * input: none
 * output: none
 * side effects: none
 * function: function to close the terminal, returns 0 on success
 */
int32_t terminal_close(){
  return 0;
}

/* terminal_write
 * input: buffer to write, bytes to write
 * output: number of bytes written
 * side effects: prints to screen
 * function: takes the buffer inputted and outputs it to the terminal screen
 */
int32_t terminal_write(const char* buf, uint32_t nbytes){
    int i;
    int bytes_written = 0;

    //wrap in critical section to prevent race condition
    cli();

    //go through each char in buffer and output it to the screen
    for(i=0;i<nbytes;i++){
        putc(buf[i]);
        bytes_written++;
    }
    sti();

    return bytes_written;
}

/* terminal_read
 * input: NONE
 * output: -1
 * side effects: NONE
 * function: Doesn't do anything
 */
int32_t terminal_read(){
    return -1;
}

int32_t terminal_driver(uint32_t cmd, int8_t* buf, uint32_t nbytes){
    if(cmd == 0){
        return terminal_open();
    }
    else if(cmd == 1){
        return terminal_read();
    }
    else if(cmd == 2){
        return terminal_write((const int8_t*)buf, nbytes);
    }
    else if(cmd == 3){
        return terminal_close();
    }
    return -1;
}
