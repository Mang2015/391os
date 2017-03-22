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
 * INPUT/OUTPUT: none
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

int32_t halt(uint8_t status){
    return 0;
}

int32_t execute(const uint8_t* command){
    return 0;
}

int32_t read(int32_t fd, void* buf, int32_t nbytes){
    return 0;
}

int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    return 0;
}

int32_t open(const uint8_t* filename){
    return 0;
}

int32_t close(int32_t fd){
    return 0;
}

int32_t getargs(uint8_t* buf, int32_t nbytes){
    return 0;
}

int32_t vidmap(uint8_t** screen_start){
    return 0;
}

int32_t set_handler(int32_t signum, void* handler_address){
    return 0;
}

int32_t sigreturn(){
    return 0;

}
