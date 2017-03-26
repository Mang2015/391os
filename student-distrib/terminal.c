#include "terminal.h"

int terminal_write(char* buf, int byte_count){
    int i;
    int bytes_written = 0;
    for(i=0;i<byte_count;i++){
      if(buf[i] == '\0')
        continue;
      else
        putc(buf[i]);
      bytes_written++;
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
