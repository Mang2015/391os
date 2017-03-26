#ifndef TERMINAL_H
#define TERMINAL_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"

int terminal_write(char* buf, int byte_count);
int terminal_read();

#endif
