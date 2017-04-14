#include "sys_handlers.h"

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
        uint8_t stat = 0xFF & arg0;
        return halt(stat);
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
    return 0;
}

/* execute
 *
 * DESCRIPTION:
 * INPUT/OUTPUT: const uint8_t* command
 * SIDE EFFECTS: none
 */
int32_t execute(const uint8_t* command){
    if(num_processes == 2)
        return -1;
    num_processes++;
    int8_t prog[4190208];
    int8_t cmd[128];
    int8_t exe[4];
    dentry_t d;
    int32_t i = 0;
    int32_t length = 0;

    while((int8_t)command[i] != ' ' || (int8_t)command[i] != '\0'){
        cmd[i] = command[i];
        i++;
    }

    cmd[i] = '\0';

    if(dread(cmd,&d) == -1 || d.ftype != 2){
        num_processes--;
        return -1;
    }
    if(fread(d.inode_num,0,exe,4) != 0){
        num_processes--;
        return -1;
    }
    //check if executable
    if(exe[0] != 0x7F || exe[1] != 0x45 || exe[2] != 0x4C || exe[3] != 0x46){
        num_processes--;
        return -1;
    }


    //getargs
    //getargs(command+i,length);

    //new paging
    if(num_processes == 1)
        page_directory[32] = 0x800000 | SURWON;//8MB
    else
        page_directory[32] = 0xC00000 | SURWON;//12MB
    //load file into memory
    int8_t *prog_ptr = 0x08048000;
    length = get_length(d.inode_num);
    if(fread(d.inode_num,0,prog,length) != length){
        num_processes--;
        return -1;
    }
    for(i = 0; i < length; i++){
        prog_ptr[i] = prog[i];
    }


    //pcb stuff
    //fill in a new task stack to bottom of kernel page
    task_stack_t *process = 0x800000 - 0x2000 * (num_processes - 1);

    //fill in child pcb
    if(num_processes != 1){
        int32_t pcb_loc;
        asm ("movl %%esp,%0":"=r"(pcb_loc));
        pcb_loc = pcb_loc & 0xFFFFF000;
        if(pcb_loc % 0x2000)
            pcb_loc -= 0x1000;

        process-->proc.parent_proc_id = num_processes-1;
        process-->proc.parent_pcb = (process_control_block_t*)pcb_loc;
    }
    process-->proc.proc_id = num_processes;
    asm ("movl %%esp,%0":"=r"(process-->proc.esp_loc));
    asm ("movl %%ebp,%0":"=r"(process-->proc.ebp_loc));


    //context switch -> iret
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
    return 0;
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
    return 0;
}

/* open
 *
 * DESCRIPTION:
 * INPUT/OUTPUT: const uint8_t* filename
 * SIDE EFFECTS: none
 */
int32_t open(const uint8_t* filename){
    return 0;
}

/* close
 *
 * DESCRIPTION:
 * INPUT/OUTPUT: int32_t fd
 * SIDE EFFECTS: none
 */
int32_t close(int32_t fd){
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
