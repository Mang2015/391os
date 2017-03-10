#include "idt.h"

void init_idt(){
    //divide by 0
    idt_desc_t int0;
    SET_IDT_ENTRY(int0,&exception_handler);
    int i;
    for(i = 0; i < NUM_VEC; i++){
        int0.val = i;
        idt[i] = int0;
    }


}

void exception_handler(){
    printf("error!");
    while(1);
}
