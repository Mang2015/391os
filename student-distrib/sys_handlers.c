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
        return halt((uint8_t)(0xFF & arg0));
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
 * DESCRIPTION: stops the system
 * INPUT/OUTPUT: uint8_t status
 * SIDE EFFECTS: none
 */
int32_t halt(uint8_t status){

    uint32_t i;

    if (num_processes == 1)
    {
        // restart shell
        execute((uint8_t*)"shell");         // need to recheck this but I think it's fine
    }

    //cli();

    // need to access current process pcb to get values for parent process
    task_stack_t *curr_process = (task_stack_t*)(0x800000 - 0x2000 * (num_processes));
    process_control_block_t *curr_block = &curr_process->proc;

    //reset pcb pointer
    curr_pcb = curr_block->parent_pcb;

    //update tss esp and ss (reloading parent data)
    tss.esp0 = curr_block->parent_esp0;
    tss.ss0 = curr_block->parent_ss0;

    //back to parent process
    num_processes--;

    //restore parent paging
    page_directory[32] = 0x800000 | SURWON;//8MB

    //flush tlb
    asm volatile(
        "movl %cr3,%eax \n \
        movl %eax,%cr3"
    );

    // change all fd flags to 0
    for (i = 2; i < 8; i++) {
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
 * DESCRIPTION:
 * INPUT/OUTPUT: const uint8_t* command
 * SIDE EFFECTS: none
 */
int32_t execute(const uint8_t* command){

    if(num_processes == 2){
        printf("Max processes already running\n");
        return -1;
    }
    num_processes++;
    //command line buffer
    int8_t cmd[128];
    int8_t exe[4];
    int8_t entry[4];
    dentry_t d;
    int32_t i = 0;
    int32_t length = 0;

    /*--------------
    PARSE ARGUMENT
    ----------------*/
    while(((int8_t)command[i] != ' ') && ((int8_t)command[i] != '\0')){
        cmd[i] = command[i];
        i++;
    }

    cmd[i] = '\0';

    if(dread(cmd,&d) == -1 || d.ftype != 2){
        num_processes--;
        return -1;
    }
    if(fread(d.inode_num,0,exe,4) != 4){
        num_processes--;
        return -1;
    }


    /*--------------
    CHECK FILE VALIDITY
    ----------------*/
    if(exe[0] != 0x7F || exe[1] != 0x45 || exe[2] != 0x4C || exe[3] != 0x46){
        num_processes--;
        return -1;
    }

    //get entry point to user level program from bytes 24-27 of executable file
    fread(d.inode_num,24,entry,4);
    //uint32_t eip_val = 0;
    uint32_t eip_val = *((uint32_t*)entry);
    /*
    eip_val = eip_val | entry[0];
    eip_val = eip_val | (entry[1] << 8);
    eip_val = eip_val | (entry[2] << 16);
    eip_val = eip_val | (entry[3] << 24);
    */

    //getargs
    //getargs(command+i,length);

    /*--------------
    SETUP PAGING
    ----------------*/
    if(num_processes == 1)
        page_directory[32] = 0x800000 | SURWON;//8MB
    else
        page_directory[32] = 0xC00000 | SURWON;//12MB

    //flush tlb
    asm volatile(
        "movl %cr3, %eax \n \
        movl %eax, %cr3");

    /*-------------------
    LOAD FILE INTO MEMORY
    ---------------------*/
    length = get_length(d.inode_num);
    int8_t *prog_ptr = (int8_t*)0x08048000;
    if(fread(d.inode_num,0,prog_ptr,length) != length){
        num_processes--;
        return -1;
    }

    //load in program instructions into address space
    /*
    for(i = 0; i < length; i++){
        prog_ptr[i] = prog[i];
    }
    */
    //memcpy(prog_ptr,prog,length);

    /*-------------
    CREATE NEW PCB
    ---------------*/

    //fill in a new task stack to bottom of kernel page
    task_stack_t *process = (task_stack_t*)(0x800000 - 0x2000 * (num_processes));

    //fill in child pcb
    if(num_processes != 1){
        process->proc.parent_pcb = (process_control_block_t*)(0x800000 - 0x2000 * (num_processes - 1));
        process->proc.parent_proc_id = num_processes-1;
        process->proc.parent_esp0 = tss.esp0;
        process->proc.parent_ss0 = tss.ss0;
    }
    process->proc.proc_id = num_processes;

    /*-----------------
    OPEN RELEVANT FD'S
    -------------------*/
    //open stdin
    process->proc.file_arr[0].flags = 1;
    process->proc.file_arr[0].table = keyboard_driver;

    //open stdout
    process->proc.file_arr[1].flags = 1;
    process->proc.file_arr[1].table = terminal_driver;

    //initialize "in use" flags to 0
    int j;
    for(j=2; j<8; j++)
        process->proc.file_arr[j].flags = 0;

    curr_pcb = &(process->proc);

    tss.esp0 = (uint32_t)process + 0x1FFC;
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
          "movl $0x2B, %eax   \n \
          pushl %eax \n \
          pushl $0x83FFFFF \n \
          pushfl \n \
          pushl $0x23"
    );

          //movw %ax, %ds \n
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
 * DESCRIPTION: read
 * INPUT/OUTPUT: int32_t fd
                void* buf
                int32_t nbytes
 * SIDE EFFECTS: none
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    if(fd < 0 || fd > 7 || curr_pcb->file_arr[fd].flags == 0)
        return -1;
    int32_t bytes_read = curr_pcb->file_arr[fd].table(READ,fd,buf,nbytes);
    if(bytes_read == -1)
        return -1;

    if(curr_pcb->file_arr[fd].flags == DIRECTORY)
        curr_pcb->file_arr[fd].position++;
    else
        curr_pcb->file_arr[fd].position += bytes_read;
    return bytes_read;
}

/* write
 *
 * DESCRIPTION:
 * INPUT/OUTPUT: int32_t fd
                const void* buf
                int32_t nbytes
 * SIDE EFFECTS: none
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    if(fd < 0 || fd > 7 || curr_pcb->file_arr[fd].flags == 0)
        return -1;
    int32_t bytes_written = curr_pcb->file_arr[fd].table(WRITE,fd,(void*)buf,nbytes);
    return bytes_written;
}

/* open
 *
 * DESCRIPTION:
 * INPUT/OUTPUT: const uint8_t* filename
 * SIDE EFFECTS: none
 */
int32_t open(const uint8_t* filename){
    int32_t i;
    dentry_t d;
    if(dread((const int8_t*)filename,&d) == -1)
        return -1;
    for(i = 2; i < 8; i++){
        if(curr_pcb->file_arr[i].flags == 0){
            curr_pcb->file_arr[i].flags = 1;
            curr_pcb->file_arr[i].inode = d.inode_num;
            curr_pcb->file_arr[i].position = 0;
            //file is rtc
            if(d.ftype == 0)
                curr_pcb->file_arr[i].table = rtc_driver;
            //file is directory
            else if(d.ftype == 1){
                curr_pcb->file_arr[i].position = get_idx(d.inode_num);
                curr_pcb->file_arr[i].table = d_driver;
                curr_pcb->file_arr[i].flags = DIRECTORY;
            }
            //file is file
            else if(d.ftype == 2)
                curr_pcb->file_arr[i].table = f_driver;

            break;
        }
    }
    if(i == 8)
        return -1;
    //call specific open
    curr_pcb->file_arr[i].table(OPEN,i,NULL,-1);
    //return fd
    return i;
}

/* close
 *
 * DESCRIPTION:
 * INPUT/OUTPUT: int32_t fd
 * SIDE EFFECTS: none
 */
int32_t close(int32_t fd){
    //invalid fd
    if(fd < 2 || fd > 7 || curr_pcb->file_arr[fd].flags == 0)
        return -1;
    //call specific close
    curr_pcb->file_arr[fd].table(CLOSE,fd,NULL,-1);
    //mark as empty
    curr_pcb->file_arr[fd].flags = 0;

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
    return 0;
}

/* vidmap
 *
 * DESCRIPTION:
 * INPUT/OUTPUT: uint8_t** screen_start
 * SIDE EFFECTS: none
 */
int32_t vidmap(uint8_t** screen_start){
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
