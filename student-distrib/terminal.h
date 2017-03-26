#ifndef TERMINAL_H
#define TERMINAL_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"

int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t terminal_read(int32_t fd, char* buf, int byte_count);

#endif
