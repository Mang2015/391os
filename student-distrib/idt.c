#include "idt.h"

void exception_handler(){
    printf("error!");
    while(1);
}

void init_idt(){

  lidt(idt_desc_ptr);

  int i;
  for(i=0;i<NUM_VEC;i++){
    idt[i].seg_selector = KERNEL_CS;
    idt[i].dpl = 0;
    idt[i].size = 1;
    idt[i].present = 1;
    idt[i].reserved0 = 0;
    idt[i].reserved1 = 1;
    idt[i].reserved2 = 1;
    idt[i].reserved3 = 1;
    idt[i].reserved4 = 0;

    if(i>=32){
      idt[i].reserved3 = 0;
    }

    if(i == 0x80)
      idt[i].dpl = 3;

      SET_IDT_ENTRY(idt[i], exception_handler);
  }

}
