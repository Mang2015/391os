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
    //max file size
    int8_t prog[4190208];
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
    while((int8_t)command[i] != ' ' || (int8_t)command[i] != '\0'){
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

    /*-------------------
    LOAD FILE INTO MEMORY
    ---------------------*/
    int8_t *prog_ptr = 0x08048000;
    length = get_length(d.inode_num);
    if(fread(d.inode_num,0,prog,length) != length){
        num_processes--;
        return -1;
    }

    //load in program instructions into address space
    /*
    for(i = 0; i < length; i++){
        prog_ptr[i] = prog[i];
    }
    */
    memcpy(prog_ptr,prog,length);

    /*-------------
    CREATE NEW PCB
    ---------------*/

    //fill in a new task stack to bottom of kernel page
    task_stack_t *process = 0x800000 - 0x2000 * (num_processes - 1);

    //fill in child pcb
    if(num_processes != 1){
        process->proc.parent_pcb = 0x800000 - 0x2000 * (num_processes - 2);
        process->proc.parent_proc_id = num_processes-1;
        process->proc.parent_esp = tss.esp0;
        process->proc.parent_ss = tss.ss0;
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
        process->proc.file_arr[i].flags = 0;


    /*--------------------------
    PUSH IRET CONTEXT TO STACK
    AND CALL IRET
    ----------------------------*/
    asm volatile(

        "movw $0x23, %%cs \n \
        movw $0x2B, %%ss \n \
        movw $0x23, %%ds \n \

        pushl $0 \n \
        pushw $cs \n \
        pushl %%eflags \n \
        pushl %%esp \n \
        pushw %%ss \n \
        movw %%esp,$0"
        :"=r"(tss.esp0)
        :"r"(eip_val)
    );

    //IRET
    asm ("iret");

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
    int32_t i;
    dentry_t d;
    if(dread(filename,d) == -1)
        return -1;
    for(i = 2; i < 8; i++){
        if()
    }
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
