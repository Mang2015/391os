#ifndef SYS_HANDLER_H_H
#define SYS_HANDLER_H_H

#include "sys_handlers.h"
#include "lib.h"
#include "fs.h"
#include "paging.h"
#include "rtc.h"
#include "terminal.h"
#include "keyboard.h"

#define SHELL0 0
#define SHELL1 1
#define SHELL2 2


void init_kernel_memory();
void init_shell();
void switch_terminal();


#endif
