#include "sys_handler_helper.h"

uint32_t vid_backups[] = {BACKUP0,BACKUP1,BACKUP2};
uint8_t buf_backup0[128];
uint8_t buf_backup1[128];
uint8_t buf_backup2[128];
uint8_t *buf_backups[] = {buf_backup0,buf_backup1,buf_backup2};
int32_t buff_idx_backups[3];
uint32_t xcoord_backups[3];
uint32_t ycoord_backups[3];

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
    if(proc_idx == 1)
        process->proc.proc_id = 4;
    if(proc_idx == 2)
        process->proc.proc_id = 8;

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
    tasks->task[0].proc.proc_id = 0;
    mem_locs[0] = TERMINAL0;
    mem_locs[1] = TERMINAL1;
    mem_locs[2] = TERMINAL2;
    mem_locs[3] = PROCESS0;
    mem_locs[4] = PROCESS1;
    mem_locs[5] = PROCESS2;
    num_processes = 0;
    curr_terminal = SHELL0;
    shell_dirty = 0x001;
}


void switch_terminal(int32_t shell){
    if(shell == curr_terminal || shell < SHELL0 || shell > SHELL2)
      return;
    //save video memory and cursor position of current task and keyboard
    xcoord_backups[curr_terminal] = coordReturn(1);
    ycoord_backups[curr_terminal] = coordReturn(0);
    buff_idx_backups[curr_terminal] = get_buf_idx();
    memcpy((void*)vid_backups[curr_terminal],(const void*)VIDEO,4096);
    memcpy((void*)buf_backups[curr_terminal],(const void*)line_char_buffer,128);
    clear();

    curr_terminal = shell;
    curr_pcb = &(tasks->task[curr_terminal].proc);

    //creating terminal for the first time
    if(((0x1 << curr_terminal) & shell_dirty) == 0){
        clear_buffer();
        resetCursor();
        shell_dirty |= 0x1 << curr_terminal;
        //save...
        //send_eoi(1);
        sti();
        system_handler(SYS_EXECUTE,(uint32_t)"shell123",0,0);
    }

    //ELSE load video memory and cursor location and keyboard
    else{
      memcpy((void*)VIDEO,(const void*)vid_backups[curr_terminal],4096);
      //resetCursor();
      //terminal_write((const char*)VIDEO,2000);
      memcpy((void*)line_char_buffer,(const void*)buf_backups[curr_terminal],128);
      set_buf_idx(buff_idx_backups[curr_terminal]);
      placeCursor(xcoord_backups[curr_terminal],ycoord_backups[curr_terminal]);
    }
  //  sti();
}
