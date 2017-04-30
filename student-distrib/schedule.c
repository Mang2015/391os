//this is for code for scheduler
#include "schedule.h"

void pit_init(void)
{
    /*
        1) Set command byte to square wave
        2) Set low bytes of divisor
        3) Set high bytes of divisor
    */

    int32_t i;

    outb(PIT_MODE_3, MODE_COMMAND_REG);
    outb(DIV_100HZ & MASK_FREQ, PIT_0_DATA_PORT);
    outb(DIV_100HZ >> 8, PIT_0_DATA_PORT);

    // enable interrupts on PIC
    enable_irq(PIT_IRQ_NUM);

    for(i = 0; i < 6; i++)
        schedule_arr[i] = NULL;
    curr = -1;
}

void pit_handler()
{
    send_eoi(PIT_IRQ_NUM);

    //move to next process
    int32_t last = curr;
    int32_t i;
    for(i = 0; i < 6; i++){
        curr = (curr+1) % 6;
        if(schedule_arr[curr])
            break;
    }

    //no processes to schedule or nothing to update
    if(last == curr){
        return;
    }

    //write code that turns on "display to screen" only for running process
    //processes running in background write to their own backups

    curr_pcb = schedule_arr[curr];

    //context switching
  /*  asm (
      "movl %%eip, %0"
      :"=r"(schedule_arr[last]->sched_eip)
    );*/
  /*  asm (
      "movl %%esp, %0"
      :"=r"(schedule_arr[last]->sched_esp)
    );
    asm (
      "movl %%ebp, %0"
      :"=r"(schedule_arr[last]->sched_ebp)
    );

  /*  asm (
      "movl %0, %%eip"
      :
      :"r"(schedule_arr[curr]->sched_eip)
    );*/
  /*  asm (
      "movl %0, %%esp"
      :
      :"r"(schedule_arr[curr]->sched_esp)
    );
    asm (
      "movl %0, %%ebp"
      :
      :"r"(schedule_arr[curr]->sched_ebp)
    );

*/
  //  tss.esp0 = (uint32_t)(curr_pcb+STACK_SIZE4);






}
