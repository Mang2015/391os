#include "terminal.h"

int terminal_write(char* buf){
    int i;
    int bytes_written = 0;
    while(buf[i] != '\0'){
      putc(buf[i]);
      bytes_written++;
      i++;
    }
    return bytes_written;
}

int terminal_read(){
  return -1;
}
