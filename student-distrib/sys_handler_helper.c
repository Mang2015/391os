#include "sys_handler_helper.h"

void init_shell(){
    if(num_processes >= 2)
        return;

    dentry_t d;
    int32_t i;
    int32_t length;
    int32_t proc_idx;
    //increment processes
    num_processes++;

    //find space for current process on kernel stack
    task_stack_t *process;
    for(i = 1; i < MAX_PROCESS; i++){
        //find first available kernel stack
        if(tasks->task[i].in_use == OFF){
           tasks->task[i].in_use = ON;
           process = &tasks->task[i];
           proc_idx = i;
           break;
        }
    }
    dread("shell",&d);

    //set up paging
    page_directory[32] = mem_locs[proc_idx] | SURWON;
    process->proc.idx = proc_idx;

    //flush tlb
    asm volatile(
        "movl %cr3, %eax \n \
        movl %eax, %cr3");


    /*-------------------
    LOAD FILE INTO MEMORY
    ---------------------*/
    length = get_length(d.inode_num);
    int8_t *prog_ptr = (int8_t*)USER_ENTRY;
    fread(d.inode_num,0,prog_ptr,length);

    //set up pcb id
    process->proc.proc_id = 0;

    //open stdin
    process->proc.file_arr[0].flags = ON;
    process->proc.file_arr[0].table = keyboard_driver;

    //open stdout
    process->proc.file_arr[1].flags = ON;
    process->proc.file_arr[1].table = terminal_driver;

    //initialize "in use" flags to 0
    for(i=2; i<MAX_FD; i++)
        process->proc.file_arr[i].flags = OFF;
}


void init_kernel_memory(){
    int32_t i;
    tasks = (kernel_tasks_t*)(KERNEL_BOT - MAX_PROCESS * STACK_SIZE);
    for(i = 0; i < MAX_PROCESS; i++){
        tasks->task[i].in_use = OFF;
    }
    mem_locs[0] = TERMINAL0;
    mem_locs[1] = TERMINAL1;
    mem_locs[2] = TERMINAL2;
    mem_locs[3] = PROCESS0;
    mem_locs[4] = PROCESS1;
    mem_locs[5] = PROCESS2;
    num_processes = 0;
}
