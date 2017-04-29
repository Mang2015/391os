//this is for code for scheduler
#include "schedule.h"

void pit_init(void)
{
    /*
        1) Set command byte to square wave
        2) Set low bytes of divisor
        3) Set high bytes of divisor
    */

    outb(PIT_MODE_3, MODE_COMMAND_REG);
    outb(DIV_100HZ & MASK_FREQ, PIT_0_DATA_PORT);
    outb(DIV_100HZ >> 8, PIT_0_DATA_PORT);

    // enable interrupts on PIC
    enable_irq(PIT_IRQ_NUM);
}

void pit_handler()
{
    //this should handle the round robin
}
