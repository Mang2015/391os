#include "rtc.h"

unsigned char cur_val;

void rtc_init(void) {

  outb(STAT_REG_B, RTC_PORT);             // Select status register B and disable NMI
  cur_val = inb(RW_CMOS);                 // load cur_val with value in status register B
  outb(STAT_REG_B, RTC_PORT);             // reset the index back into RTC_PORT
  outb(cur_val | ENABLE_BIT_SIX, RW_CMOS);  // "OR" current value with 0x40 and write into CMOS port


  enable_irq(RTC_IRQ_NUM);                // enable on PIC

  /* need to figure out how to change rate, am working on that */

}

void rtc_handler() {

  test_interrupts();

  send_eoi(RTC_IRQ_NUM);
}
