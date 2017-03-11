#include "idt.h"

void exception_handler(){
    printf("error!");
    while(1);
}

/* init_idt
 *
 * DESCRIPTION: Initializes IDT by looping throught table and setting
 *              corresponding descriptor bits. Also sets offsets to
 *              for various entries in the table.
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: fills IDT with corresponding descriptor values and offsets
 */
void init_idt(){

  //load the pointer to idt into the IDTR
  lidt(idt_desc_ptr);

  int i;
  //loop through each vector in the idt and set the corresponding
  //bits in the descriptor
  for(i=0;i<NUM_VEC;i++){

    idt[i].seg_selector = KERNEL_CS;
    //priority level should be 0 for non system calls
    idt[i].dpl = 0;

    idt[i].size = 1;
    idt[i].present = 1;

    //mark every descriptor as 01100 in bit values
    idt[i].reserved0 = 0;
    idt[i].reserved1 = 1;
    idt[i].reserved2 = 1;
    idt[i].reserved3 = 0;
    idt[i].reserved4 = 0;

    //if system call, priority level is 3
    if(i == 0x80)
      idt[i].dpl = 3;

      SET_IDT_ENTRY(idt[i], exception_handler);
  }

}
