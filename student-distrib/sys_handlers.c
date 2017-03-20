#include "sys_handlers.h"

int32_t halt();
int32_t execute();
int32_t read();
int32_t write();
int32_t open();
int32_t close();
int32_t getargs();
int32_t vidmap();
int32_t set_handler();
int32_t sigreturn();


/* system_handler
 *
 * DESCRIPTION: INT 80 was invoked
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: none
 */
void system_handler(){
    int32_t instr;
    asm volatile("movl %%eax,%0":"=r"(instr));

    if(instr == SYS_HALT){
        halt();
    }
    else if(instr == SYS_EXECUTE){
        execute();
    }
    else if(instr == SYS_READ){
        read();
    }
    else if(instr == SYS_WRITE){
        write();
    }
    else if(instr == SYS_OPEN){
        open();
    }
    else if(instr == SYS_CLOSE){
        close();
    }
    else if(instr == SYS_GETARGS){
        getargs();
    }
    else if(instr == SYS_VIDMAP){
        vidmap();
    }
    else if(instr == SYS_SET_HANDLER){
        set_handler();
    }
    else if(instr == SYS_SIGRETURN){
        sigreturn();
    }



}

int32_t halt(){
    uint8_t status;
    asm volatile("movb %%bl,%0":"=r"(status));
}

int32_t execute(){
    uint8_t* command;
    asm volatile("movl %%ebx,%0":"=r"(command));
}

int32_t read(){
    int32_t fd;
    void* buf;
    int32_t nbytes;

    asm volatile("movl %%ebx,%0":"=r"(fd));
    asm volatile("movl %%ecx,%0":"=r"(buf));
    asm volatile("movl %%edx,%0":"=r"(nbytes));
}

int32_t write(){
    int32_t fd;
    void* buf;//constant
    int32_t nybtes;

    asm volatile("movl %%ebx,%0":"=r"(fd));
    asm volatile("movl %%ecx,%0":"=r"(buf));
    asm volatile("movl %%edx,%0":"=r"(nbytes));
}

int32_t open(){
    uint8_t* filename;

    asm volatile("movl %%ebx,%0":"=r"(filename));
}

int32_t close(){
    int32_t fd;

    asm volatile("movl %%ebx,%0":"=r"(fd));

}

int32_t getargs(){
    uint8_t *buf;
    int32_t nbytes;
    asm volatile("movl %%ebx,%0":"=r"(buf));
    asm volatile("movl %%ecx,%0":"=r"(nbytes));
}

int32_t vidmap(){
    uint8_t **screen_start;

    asm volatile("movl %%ebx,%0":"=r"(screen_start));
}

int32_t set_handler(){
    int32_t signum;
    void* handler_address;
    asm volatile("movl %%ebx,%0":"=r"(signum));
    asm volatile("movl %%ecx,%0":"=r"(handler_address));
}

int32_t sigreturn(){

}
