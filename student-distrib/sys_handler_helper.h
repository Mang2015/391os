#ifndef SYS_HANDLER_H_H
#define SYS_HANDLER_H_H

#include "sys_handlers.h"
#include "lib.h"
#include "fs.h"
#include "paging.h"
#include "rtc.h"
#include "terminal.h"
#include "keyboard.h"

#define SHELL1 0
#define SHELL2 1


void init_kernel_memory();
void init_shell();


#endif
