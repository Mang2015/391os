#include "idt.h"

//static array of all system handlers
static uint32_t sys_handlers[NUM_SYS_HANDLERS] = {
    (uint32_t)divide_handler,
    (uint32_t)debug_handler,
    (uint32_t)nmi_interrupt_handler,
    (uint32_t)breakpoint_handler,
    (uint32_t)overflow_handler,
    (uint32_t)bound_range_handler,
    (uint32_t)invalid_opcode_handler,
    (uint32_t)device_not_avail_handler,
    (uint32_t)dbl_fault_handler,
    (uint32_t)coprocess_seg_handler,
    (uint32_t)inval_tss_handler,
    (uint32_t)seg_not_pres_handler,
    (uint32_t)stack_fault_handler,
    (uint32_t)gen_protect_handler,
    (uint32_t)page_fault_handler,
    (uint32_t)exception_handler,//general exception here
    (uint32_t)float_point_handler,
    (uint32_t)align_check_handler,
    (uint32_t)machine_check_handler,
    (uint32_t)simd_float_point_handler
};

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
    idt[i].dpl = RING0;

    idt[i].size = 1;
    idt[i].present = 1;

    //mark every descriptor as 01110 in bit values to signify interrupt gate
    idt[i].reserved0 = 0;
    idt[i].reserved1 = 1;
    idt[i].reserved2 = 1;
    idt[i].reserved3 = 0;
    idt[i].reserved4 = 0;

    //if system call, priority level is 3
    if(i == SYS_CALL)
      idt[i].dpl = RING3;

    //assign correct handler
    if(i < NUM_SYS_HANDLERS)
        SET_IDT_ENTRY(idt[i],sys_handlers[i]);
    else
        SET_IDT_ENTRY(idt[i],exception_handler);

  }

}

/* exception_handler
 *
 * DESCRIPTION: General exception handler
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: none
 */
void exception_handler(){
    clear();
    printf("Unknown error occured");
    while(1);
}

/* divide_handler
 *
 * DESCRIPTION: Handler for Divide Error Exception (#DE)
                Called upon a divide by 0 or result would overflow
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: none
 */
void divide_handler(){
    clear();
    printf("Interrupt 0 - Divide Error Exception\n");
    while(1);
}

/* debug_handler
 *
 * DESCRIPTION: Handler for Divide Error Exception (#DE)
                Called upon a divide by 0 or result would overflow
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: none
 */
void debug_handler(){
    clear();
    printf("Interrupt 1 - Debug Exception\n");
    while(1);
}

void nmi_interrupt_handler(){
    clear();
    printf("Interrupt 2 - Nonmaskable Interrupt\n");
    while(1);
}

void breakpoint_handler(){
    clear();
    printf("Interrupt 3 - Breakpoint Exception\n");
    while(1);
}

void overflow_handler(){
    clear();
    printf("Interrupt 4 - Overflow Exception\n");
    while(1);
}

void bound_range_handler(){
    clear();
    printf("Interrupt 5 - BOUND Range Exceeded\n");
    while(1);
}

void invalid_opcode_handler(){
    clear();
    printf("Interrupt 6 - Invalid Opcode\n");
    while(1);
}

void device_not_avail_handler(){
    clear();
    printf("Interrupt 7 - Device Not Available\n");
    while(1);
}

void dbl_fault_handler(){
    clear();
    printf("Interrupt 8 - Double Fault\n");
    while(1);
}

void coprocess_seg_handler(){
    clear();
    printf("Interrupt 9 - Coprocessor Segment Overrun\n");
    while(1);
}

void inval_tss_handler(){
    clear();
    printf("Interrupt 10 - Invalid TSS\n");
    while(1);
}

void seg_not_pres_handler(){
    clear();
    printf("Interrupt 11 - Segment Not Present\n");
    while(1);
}

void stack_fault_handler(){
    clear();
    printf("Interrupt 12 - Stack Fault Exception\n");
    while(1);
}

void gen_protect_handler(){
    clear();
    printf("Interrupt 13 - General Protection Exception\n");
    while(1);
}

void page_fault_handler(){
    clear();
    printf("Interrupt 14 - Page-Fault Exception\n");
    while(1);
}

void float_point_handler(){
    clear();
    printf("Interrupt 16 - x87 FPU Floating-Point Error\n");
    while(1);
}

void align_check_handler(){
    clear();
    printf("Interrupt 17 - Alignment Check Exception\n");
    while(1);
}

void machine_check_handler(){
    clear();
    printf("Interrupt 18 - Machine-Check Exception\n");
    while(1);
}

void simd_float_point_handler(){
    clear();
    printf("Interrupt 19 - SIMD Floating-Point Exception\n");
    while(1);
}
