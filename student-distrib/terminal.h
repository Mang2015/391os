#ifndef TERMINAL_H
#define TERMINAL_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"

int32_t terminal_open();
int32_t terminal_close();
int32_t terminal_write(const char* buf, uint32_t nbytes);
int32_t terminal_read(void* buf, uint32_t byte_count);
int32_t terminal_driver(uint32_t cmd, int8_t* buf, uint32_t nbytes);

#endif
