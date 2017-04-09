#ifndef SYS_HANDLERS
#define SYS_HANDLERS

#include "lib.h"
#include "fs.h"

#define SYS_HALT    1
#define SYS_EXECUTE 2
#define SYS_READ    3
#define SYS_WRITE   4
#define SYS_OPEN    5
#define SYS_CLOSE   6
#define SYS_GETARGS 7
#define SYS_VIDMAP  8
#define SYS_SET_HANDLER  9
#define SYS_SIGRETURN  10

int32_t system_handler(uint32_t instr, uint32_t arg0, uint32_t arg1, uint32_t arg2);


#endif
