//this is for code for scheduler
#include "schedule.h"

uint32_t sched_vid_backups[] = {BACKUP0,BACKUP1,BACKUP2};

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
    for(i = 0; i < 3; i++){
        schedule_arr[i] = NULL;
        new_process[i] = 1;
    }

    curr = 0;
}



void pit_handler()
{
    send_eoi(PIT_IRQ_NUM);
    if(!setup)
        return;
 //   uint32_t flags;
  //  cli_and_save(flags);

    //move to next process
    int32_t last = curr;
    int32_t i;
    for(i = 0; i < 3; i++){
        curr = (curr+1) % 3;
        if(schedule_arr[curr])
            break;
    }

    /*if(new_process[curr]){
        new_process[curr] = 0;
        return;
    }*/
    //no processes to schedule or nothing to update
    if(last == curr || !schedule_arr[curr]){
        return;
    }
    /*
    if(schedule_arr[curr]->proc_id == 0 || schedule_arr[curr]->proc_id == 4
        || schedule_arr[curr]->proc_id == 8){
            if(schedule_arr[curr]->proc_id/4 != curr_terminal)
                return;
        }
        */



    //write code that turns on "display to screen" only for running process
    //processes running in background write to their own backups

/*
    if((curr_pcb->proc_id)/4 == curr_terminal){
        page_table[VIDEO>>12] = VIDEO | RWON;
    }else{

        int term_to_write = (curr_pcb->proc_id)/4;
        page_table[(VIDEO>>12)] = sched_vid_backups[term_to_write] | RWON;

    }

     //flush tlb
    asm volatile(
        "movl %cr3,%eax \n \
        movl %eax,%cr3"
    );
    */



    //context switching
    tss.esp0 = (uint32_t)(schedule_arr[curr])+STACK_SIZE4;
    tss.ss0 = KERNEL_DS;


    if(schedule_arr[last]){
        asm (
          "movl %%esp, %0"
          :"=r"(schedule_arr[last]->sched_esp)
        );
        asm (
          "movl %%ebp, %0"
          :"=r"(schedule_arr[last]->sched_ebp)
        );
    }


    asm (
      "movl %0, %%esp"
      :
      :"r"(schedule_arr[curr]->sched_esp)
    );
    asm (
      "movl %0, %%ebp"
      :
      :"r"(schedule_arr[curr]->sched_ebp)
    );


    page_directory[32] = mem_locs[schedule_arr[curr]->idx] | SURWON;

    //flush tlb
    asm volatile(
        "movl %cr3,%eax \n \
        movl %eax,%cr3"
    );
    curr_pcb = schedule_arr[curr];

   // restore_flags(flags);
}
