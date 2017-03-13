#include "rtc.h"

uint8_t cur_val;
uint16_t i = 0;

/* rtc_init
 *
 * DESCRIPTION: Initializes RTC and enables its respective IRQ on the PIC
 *              Enables bit 6 in status register b to turn on periodic interrupts
 *
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: Rewrites few bits in status registers A and B to enable periodic interrupts and set rate
 */
void rtc_init(void) {
  uint32_t flags;
  cli_and_save(flags);

  outb(STAT_REG_B, RTC_PORT);             // Select status register B and disable NMI
  cur_val = inb(RW_CMOS);                 // load cur_val with value in status register B
  outb(STAT_REG_B, RTC_PORT);             // reset the index back into RTC_PORT
  outb(cur_val | ENABLE_BIT_SIX, RW_CMOS);  // "OR" current value with 0x40 and write into CMOS port

  /* need to figure out how to change rate, am working on that */
  outb(STAT_REG_A, RTC_PORT);             // Select status register B and disable NMI
  cur_val = inb(RW_CMOS);                 // load cur_val with value in status register B
  outb(STAT_REG_A, RTC_PORT);             // reset the index back into RTC_PORT
  outb((cur_val & 0xF0) | RATE, RW_CMOS);


  enable_irq(RTC_IRQ_NUM);                // enable on PIC

  restore_flags(flags);
}

/* rtc_handler
 *
 * DESCRIPTION: Calls on test_interrupts function given for checkpoint 1. This function
 *              is called by IDT and handles RTC interrupts
 *
 * INPUT/OUTPUT: none
 * SIDE EFFECTS: Clears content in register C to enable new interrupts. Send EOI to PIC
 */
void rtc_handler() {
  //printf("%d",i++);
  uint32_t flags;
//  save_flags(flags);

  //printf("rtc");
  test_interrupts();
  outb(STAT_REG_C,RTC_PORT);
  inb(RW_CMOS);

  send_eoi(RTC_IRQ_NUM);

 // restore_flags(flags);
  //iret();
}
