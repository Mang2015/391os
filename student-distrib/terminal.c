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

/*In the
ase of the keyboard, read should return data from one line that has been terminated by pressing
Enter, or as mu
h as ts in the buffer from one su
h line.*/
int terminal_read(){
  return -1;
}
