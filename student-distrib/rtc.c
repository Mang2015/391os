#include "rtc.h"

uint8_t cur_val;
uint16_t i = 0;

void rtc_init(void) {
  uint32_t flags;
  cli_and_save(flags);

  outb(STAT_REG_B, RTC_PORT);             // Select status register B and disable NMI
  cur_val = inb(RW_CMOS);                 // load cur_val with value in status register B
  outb(STAT_REG_B, RTC_PORT);             // reset the index back into RTC_PORT
  outb(cur_val | ENABLE_BIT_SIX, RW_CMOS);  // "OR" current value with 0x40 and write into CMOS port

  /* need to figure out how to change rate, am working on that */
/*  outb(STAT_REG_A, RTC_PORT);             // Select status register B and disable NMI
  cur_val = inb(RW_CMOS);                 // load cur_val with value in status register B
  outb(STAT_REG_A, RTC_PORT);             // reset the index back into RTC_PORT
  outb((cur_val & 0xF0) | RATE, RW_CMOS);
*/

  enable_irq(RTC_IRQ_NUM);                // enable on PIC

  restore_flags(flags);
}

void rtc_handler() {
  //printf("%d",i++);
  uint32_t flags;
//  save_flags(flags);

  printf("rtc");
  //test_interrupts();
  outb(STAT_REG_C,RTC_PORT);
  inb(RW_CMOS);

  send_eoi(RTC_IRQ_NUM);

 // restore_flags(flags);
  //iret();
}
