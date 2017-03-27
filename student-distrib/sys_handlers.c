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




/* system_handler
 *
 * DESCRIPTION: INT 80 was invoked
 * INPUT/OUTPUT: arguments passed in through registers eax,ebx,ecx,edx
 * SIDE EFFECTS: none
 */
void system_handler(){
    uint32_t instr,arg0,arg1,arg2;
    asm ("movl %%eax,%0":"=r"(instr));
    asm ("movl %%ebx,%0":"=r"(arg0));
    asm ("movl %%ecx,%0":"=r"(arg1));
    asm ("movl %%edx,%0":"=r"(arg2));

    if(instr == SYS_HALT){
        uint8_t stat = 0xFF & arg0;
        halt(stat);
    }
    else if(instr == SYS_EXECUTE){
        execute((const uint8_t*)arg0);
    }
    else if(instr == SYS_READ){
        read((int32_t)arg0,(void*)arg1,(int32_t)arg2);
    }
    else if(instr == SYS_WRITE){
        write((int32_t)arg0,(const void*)arg1,(int32_t)arg2);
    }
    else if(instr == SYS_OPEN){
        open((const uint8_t*)arg0);
    }
    else if(instr == SYS_CLOSE){
        close((int32_t)arg0);
    }
    else if(instr == SYS_GETARGS){
        getargs((uint8_t*)arg0,(int32_t)arg1);
    }
    else if(instr == SYS_VIDMAP){
        vidmap((uint8_t**)arg0);
    }
    else if(instr == SYS_SET_HANDLER){
        set_handler((int32_t)arg0,(void*)arg1);
    }
    else if(instr == SYS_SIGRETURN){
        sigreturn();
    }
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
