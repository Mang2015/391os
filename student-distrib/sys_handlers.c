#include "sys_handlers.h"
#include "x86_desc.h"

static int32_t halt(uint8_t status);
static int32_t execute(const uint8_t* command);
static int32_t read(int32_t fd, void* buf, int32_t nbytes);
static int32_t write(int32_t fd, const void* buf, int32_t nbytes);
static int32_t open(const uint8_t* filename);
static int32_t close(int32_t fd);
static int32_t getargs(uint8_t* buf, int32_t nbytes);
static int32_t vidmap(uint8_t** screen_start);
static int32_t set_handler(int32_t signum, void* handler_address);
static int32_t sigreturn(void);


static int32_t num_processes = 0;

/* system_handler
 *
 * DESCRIPTION: INT 80 was invoked
 * INPUT/OUTPUT: arguments passed in through registers eax,ebx,ecx,edx
 * SIDE EFFECTS: none
 */
int32_t system_handler(uint32_t instr, uint32_t arg0, uint32_t arg1, uint32_t arg2){
    /*uint32_t instr,arg0,arg1,arg2;
    asm ("movl %%eax,%0":"=r"(instr));
    asm ("movl %%ebx,%0":"=r"(arg0));
    asm ("movl %%ecx,%0":"=r"(arg1));
    asm ("movl %%edx,%0":"=r"(arg2));*/

    if(instr == SYS_HALT){
        return halt((uint8_t)(BYTE & arg0));
    }
    else if(instr == SYS_EXECUTE){
        return execute((const uint8_t*)arg0);
    }
    else if(instr == SYS_READ){
        return read((int32_t)arg0,(void*)arg1,(int32_t)arg2);
    }
    else if(instr == SYS_WRITE){
        return write((int32_t)arg0,(const void*)arg1,(int32_t)arg2);
    }
    else if(instr == SYS_OPEN){
        return open((const uint8_t*)arg0);
    }
    else if(instr == SYS_CLOSE){
        return close((int32_t)arg0);
    }
    else if(instr == SYS_GETARGS){
        return getargs((uint8_t*)arg0,(int32_t)arg1);
    }
    else if(instr == SYS_VIDMAP){
        return vidmap((uint8_t**)arg0);
    }
    else if(instr == SYS_SET_HANDLER){
        return set_handler((int32_t)arg0,(void*)arg1);
    }
    else if(instr == SYS_SIGRETURN){
        return sigreturn();
    }
    return -1;
}

/* halt
 *
 * DESCRIPTION: Stops the current process
 * INPUT/OUTPUT: uint8_t status
 * SIDE EFFECTS: Decreases number of processes run, if at minimun number, restart shell
 */
int32_t halt(uint8_t status){

    uint32_t i;

    if (num_processes == 1)
    {
        // restart shell
        printf("Restarting shell...\n");
        execute((uint8_t*)"shell123");         // need to recheck this but I think it's fine
    }

    //cli();

    // need to access current process pcb to get values for parent process
    task_stack_t *curr_process = (task_stack_t*)(KERNEL_BOT - STACK_SIZE * (num_processes));
    process_control_block_t *curr_block = &curr_process->proc;

    //reset pcb pointer
    curr_pcb = curr_block->parent_pcb;

    //update tss esp and ss (reloading parent data)
    tss.esp0 = curr_block->parent_esp0;
    tss.ss0 = curr_block->parent_ss0;

    //back to parent process
    num_processes--;

    //restore parent paging
    page_directory[32] = PROCESS_PAGE1 | SURWON;//8MB

    //flush tlb
    asm volatile(
        "movl %cr3,%eax \n \
        movl %eax,%cr3"
    );

    // change all fd flags to 0
    for (i = 2; i < MAX_FD; i++) {
        if (curr_block->file_arr[i].flags != 0) {
            close(i);
        }
    }

   //sti();
   asm volatile(
       "movl %0, %%eax \n \
       addl $-4,%%eax \n \
       xorl %%ebx, %%ebx \n \
       movb %1, %%bl \n \
       movl %%ebx,(%%eax)"
       :
       :"r"(curr_block->parent_ebp),"r"(status)
       :"%eax"
   );

    // jump to execute return
    asm volatile(
        "movl %0, %%esp"
        :
        :"r"(curr_block->parent_esp)
    );
    asm volatile(
        "movl %0, %%ebp"
        :
        :"r"(curr_block->parent_ebp)
    );
    asm volatile(
        "jmp exec_ret"
    );

    //this is never actually used
    return 0;
}

/* execute
 *
 * DESCRIPTION: Creates a new process based off command given
 * INPUT/OUTPUT: const uint8_t* command
 *               Returns -1 if can't create, 0-255 if user halts,
 *               256 if exception is thrown
 * SIDE EFFECTS: Creates a new process, changes paging
 */
int32_t execute(const uint8_t* command){

    if(command == NULL)
      return -1;

    if(num_processes == MAX_PROCESS){
        printf("Max processes already running\n");
        return -1;
    }
    num_processes++;
    //command line buffer
    int8_t cmd[CMD_BUF];
    int8_t exe[BUF4];
    int8_t entry[BUF4];
    dentry_t d;
    int32_t i = 0, j;
    int32_t length = 0;

    /*--------------
    PARSE ARGUMENT
    ----------------*/
    if(strncmp((int8_t*)command,"shell123",RESTART_SIZE) == 0){
        num_processes--;
        cmd[0] = 's';
        cmd[1] = 'h';
        cmd[2] = 'e';
        cmd[3] = 'l';
        cmd[4] = 'l';
        cmd[5] = '\0';
    }
    else{
        while(((int8_t)command[i] != ' ') && ((int8_t)command[i] != '\0') && ((int8_t)command[i] != '\n')){
            cmd[i] = command[i];
            i++;
        }
          cmd[i] = '\0';
    }

    //get crrent process
    task_stack_t *process = (task_stack_t*)(KERNEL_BOT - STACK_SIZE * (num_processes));

    //copy arguments of the command into the argument pcb buffer
    strncpy(process->proc.arguments,(const int8_t*)(command+i+1),128);
    j = 0;
    while(process->proc.arguments[j] != '\n'){
        j++;
    }
    process->proc.arguments[j] = '\0';

    if(dread(cmd,&d) == -1 || d.ftype != FILE_TYPE){
        num_processes--;
        return -1;
    }
    if(fread(d.inode_num,0,exe,BUF4) != BUF4){
        num_processes--;
        return -1;
    }


    /*--------------
    CHECK FILE VALIDITY
    ----------------*/
    if(exe[0] != EXE0 || exe[1] != EXE1 || exe[2] != EXE2 || exe[3] != EXE3){
        num_processes--;
        return -1;
    }

    //get entry point to user level program from bytes 24-27 of executable file
    fread(d.inode_num,ENTRY_OFF,entry,BUF4);
    //uint32_t eip_val = 0;
    uint32_t eip_val = *((uint32_t*)entry);


    /*--------------
    SETUP PAGING
    ----------------*/
    if(num_processes == 1)
        page_directory[32] = PROCESS_PAGE1 | SURWON;//8MB
    else
        page_directory[32] = PROCESS_PAGE2 | SURWON;//12MB

    //flush tlb
    asm volatile(
        "movl %cr3, %eax \n \
        movl %eax, %cr3");

    /*-------------------
    LOAD FILE INTO MEMORY
    ---------------------*/
    length = get_length(d.inode_num);
    int8_t *prog_ptr = (int8_t*)USER_ENTRY;
    if(fread(d.inode_num,0,prog_ptr,length) != length){
        num_processes--;
        return -1;
    }


    /*-------------
    CREATE NEW PCB
    ---------------*/

    //fill in child pcb
    if(num_processes != 1){
        process->proc.parent_pcb = (process_control_block_t*)(KERNEL_BOT - STACK_SIZE * (num_processes - 1));
        process->proc.parent_proc_id = num_processes-1;
        process->proc.parent_esp0 = tss.esp0;
        process->proc.parent_ss0 = tss.ss0;
    }
    process->proc.proc_id = num_processes;

    /*-----------------
    OPEN RELEVANT FD'S
    -------------------*/
    //open stdin
    process->proc.file_arr[0].flags = ON;
    process->proc.file_arr[0].table = keyboard_driver;

    //open stdout
    process->proc.file_arr[1].flags = ON;
    process->proc.file_arr[1].table = terminal_driver;

    //initialize "in use" flags to 0
    for(j=2; j<MAX_FD; j++)
        process->proc.file_arr[j].flags = OFF;

    curr_pcb = &(process->proc);

    tss.esp0 = (uint32_t)process + STACK_SIZE4;
    tss.ss0 = KERNEL_DS;

    //save current esp and ebp to pcb
    asm volatile(
        "movl %%ebp, %0 \n \
        movl %%esp, %1"
        : "=r"(process->proc.parent_ebp),
          "=r"(process->proc.parent_esp)
    );



    /*--------------------------
    PUSH IRET CONTEXT TO STACK
    AND CALL IRET
    ----------------------------*/

    asm volatile(
          "switch: \n \
          movl $0x2B, %eax   \n \
          pushl %eax \n \
          pushl $0x83FFFFF \n \
          pushfl \n \
          pushl $0x23"
    );

    asm volatile(
        "movl %0, %%ecx \n \
        pushl %%ecx"
        :
        :"r"(eip_val)
    );

    //IRET
    asm ("iret");

    //never is used
    return 0;
}

/* read
 *
 * DESCRIPTION: reads into buffer
 * INPUT/OUTPUT: int32_t fd
                void* buf
                int32_t nbytes
                returns -1 if couldn't read, otherwise number of bytes read
 * SIDE EFFECTS: fills in buffer that was passed in
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    if(fd < 0 || fd >= MAX_FD || curr_pcb->file_arr[fd].flags == OFF)
        return -1;
    return curr_pcb->file_arr[fd].table(READ,fd,buf,nbytes);
}

/* write
 *
 * DESCRIPTION: writes into file given buffer
 * INPUT/OUTPUT: int32_t fd
                const void* buf
                int32_t nbytes
                returns -1 if couldn't write
 * SIDE EFFECTS: none
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    if(fd < 0 || fd >= MAX_FD || curr_pcb->file_arr[fd].flags == OFF)
        return -1;
    return curr_pcb->file_arr[fd].table(WRITE,fd,(void*)buf,nbytes);
}

/* open
 *
 * DESCRIPTION: Opens a new file descriptor structure to a max of 8 files
 * INPUT/OUTPUT: const uint8_t* filename
 * SIDE EFFECTS: alters fd structure in PCB
 */
int32_t open(const uint8_t* filename){
    int32_t i;
    dentry_t d;
    if(dread((const int8_t*)filename,&d) == -1)
        return -1;
    for(i = 2; i < MAX_FD; i++){
        if(curr_pcb->file_arr[i].flags == OFF){
            curr_pcb->file_arr[i].flags = ON;
            curr_pcb->file_arr[i].inode = d.inode_num;
            curr_pcb->file_arr[i].position = 0;
            //file is rtc
            if(d.ftype == RTC_TYPE)
                curr_pcb->file_arr[i].table = rtc_driver;
            //file is directory
            else if(d.ftype == DIR_TYPE){
                curr_pcb->file_arr[i].position = get_idx(d.inode_num);
                curr_pcb->file_arr[i].table = d_driver;
                curr_pcb->file_arr[i].flags = DIRECTORY;
            }
            //file is file
            else if(d.ftype == FILE_TYPE)
                curr_pcb->file_arr[i].table = f_driver;

            break;
        }
    }
    if(i == MAX_FD)
        return -1;
    //call specific open
    curr_pcb->file_arr[i].table(OPEN,i,NULL,-1);
    //return fd
    return i;
}

/* close
 *
 * DESCRIPTION: Closes fd object in PCB structure
 * INPUT/OUTPUT: int32_t fd
                 Returns -1 if invalid fd and couldn't close, otherwise 0
 * SIDE EFFECTS: none
 */
int32_t close(int32_t fd){
    //invalid fd
    if(fd < 2 || fd >= MAX_FD || curr_pcb->file_arr[fd].flags == OFF)
        return -1;
    //call specific close
    curr_pcb->file_arr[fd].table(CLOSE,fd,NULL,-1);
    //mark as empty
    curr_pcb->file_arr[fd].flags = OFF;

    return 0;
}

/* getargs
 *
 * DESCRIPTION:
 * INPUT/OUTPUT: uint8_t* buf
                 int32_t nbytes
 * SIDE EFFECTS: none
 */
int32_t getargs(uint8_t* buf, int32_t nbytes){

    //error check
    if(buf == NULL)
      return -1;
    if(nbytes > 128) nbytes = 128;

    //arguments don't exist
    if(curr_pcb->arguments[0] == '\0')
        return -1;

    //copy over the argument buffer into the passed in user level buffer
    strncpy((int8_t*)buf,curr_pcb->arguments,nbytes);

    return 0;
}

/* vidmap
 *
 * DESCRIPTION:
 * INPUT/OUTPUT: uint8_t** screen_start
 * SIDE EFFECTS: none
 */
int32_t vidmap(uint8_t** screen_start){

    //uint32_t index = 0x08400000 / KERNEL;

    //check bounds of the user level screenstart pointer
    if(screen_start == NULL || (uint32_t)screen_start < USER_ENTRY || (uint32_t)screen_start >= OOB){
      return -1;
    }

    //initialize page
    page_directory[VIDMAP_PAGE] = (uint32_t)page_table | URWON;
    page_table[0] = (uint32_t)VIDEO | URWON;

    //flush tlb
    asm volatile(
        "movl %cr3, %eax \n \
        movl %eax, %cr3"
    );

    //assign pointer to the start of video memory
    *screen_start = (uint8_t*)VIDMEM;

    return 0;
}

/* set_handler
 *
 * DESCRIPTION:
 * INPUT/OUTPUT: int32_t signum
                void* handler_address
 * SIDE EFFECTS: none
 */
int32_t set_handler(int32_t signum, void* handler_address){
    return 0;
}

/* sigreturn
 *
 * DESCRIPTION:
 * INPUT/OUTPUT:
 * SIDE EFFECTS: none
 */
int32_t sigreturn(){
    return 0;

}
