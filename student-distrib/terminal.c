#include "terminal.h"

int32_t terminal_open(){
  return 0;
}

int32_t terminal_close(){
  return 0;
}

int32_t terminal_write(const void* buf, int32_t nbytes){
    int i;
    int bytes_written = 0;

    cli();
    /*for(i=0;i<nbytes;i++){
      if(buf[i] == '\0')
        continue;
      else
        putc(buf[i]);
      bytes_written++;
    }*/
    bytes_written = printf((int8_t *)buf);
    sti();

    return bytes_written;
}

/*In the case of the keyboard, read should return data from one line that has been terminated by pressing
Enter, or as much as ts in the buffer from one such line.*/
int32_t terminal_read(char* buf, int byte_count){
    int totalBufNum = get_buf_idx();
    int i;
    for(i=0;i<=totalBufNum;i++){
      line_char_buffer[i] = buf[i];
      totalwritten++;
    }

    return i;
}
