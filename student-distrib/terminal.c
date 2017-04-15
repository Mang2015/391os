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
int32_t terminal_write(const char* buf, int32_t nbytes){
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
 * input: the buffer to write to, bytes to write
 * output: the total number of bytes written
 * side effects: writes to a buffer whatever is in the keyboard buffer
 * function: takes the keyboard buffer and copies whatever is in it to a different
 *            buffer passed in by the function
 */
int32_t terminal_read(void* buf, int byte_count){
    int totalBufNum = get_buf_idx();
    int i;
    for(i=0;i<=totalBufNum;i++){
      buf[i] = line_char_buffer[i];
    }

    return i;
}
