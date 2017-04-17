#ifndef SYS_HANDLERS
#define SYS_HANDLERS

#include "lib.h"
#include "fs.h"
#include "paging.h"
#include "rtc.h"
#include "terminal.h"
#include "keyboard.h"

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
#define USER 0x08000000
#define ON 1
#define OFF 0
#define DIRECTORY 2
#define BYTE 0xFF
#define KERNEL_BOT 0x800000
#define PROCESS_PAGE1 0x800000
#define PROCESS_PAGE2 0xC00000
#define USER_ENTRY 0x08048000
#define STACK_SIZE 0x2000
#define STACK_SIZE4 0x1FFC
#define MAX_FD 8
#define MAX_PROCESS 2
#define BUF4 4
#define CMD_BUF 128
#define RESTART_SIZE 8
#define EXE0 0x7F
#define EXE1 0x45
#define EXE2 0x4C
#define EXE3 0x46
#define ENTRY_OFF 24

#define OPEN 0
#define READ 1
#define WRITE 2
#define CLOSE 3

int32_t system_handler(uint32_t instr, uint32_t arg0, uint32_t arg1, uint32_t arg2);


typedef struct file_descriptor_structure{
    int32_t (*table)(uint32_t,uint32_t,void*,uint32_t);
    int32_t inode;
    int32_t position;
    int32_t flags;
}file_descriptor_structure_t;//16

typedef struct pcb{
    int32_t proc_id;//4
    int32_t parent_proc_id;//4
    file_descriptor_structure_t file_arr[8];//128
    struct pcb* parent_pcb;//4
    int32_t parent_esp0;//4
    int16_t parent_ss0;//2
    int16_t reserved;//2
    int32_t parent_esp;//4
    int32_t parent_ebp;//4
}process_control_block_t;//156

typedef struct task_stack{//8kb
    //pcb
    process_control_block_t proc;//146
    int8_t stack[8192-156];
}task_stack_t;

process_control_block_t *curr_pcb;

#endif
