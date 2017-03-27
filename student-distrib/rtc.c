#include "rtc.h"

uint8_t cur_val;
uint8_t disp_handler;

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
  outb(STAT_REG_A, RTC_PORT);             // Select status register A and disable NMI
  cur_val = inb(RW_CMOS);                 // load cur_val with value in status register A
  outb(STAT_REG_A, RTC_PORT);             // reset the index back into RTC_PORT
  outb((cur_val & 0xF0) | RATE, RW_CMOS);

  int_flag = 0;
  disp_handler = 0;

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

  //test_interrupts();
  outb(STAT_REG_C,RTC_PORT);
  inb(RW_CMOS);

  if(disp_handler) putc('1');
  send_eoi(RTC_IRQ_NUM);
  int_flag = 1; //set int_flag for read_rtc
}

int32_t open_rtc(const uint8_t* filename)
{
    set_freq(2);

    return 0;
}

int32_t read_rtc(int32_t fd, void* buf, int32_t nbytes)
{
    while(!int_flag)
    {
        //wait for interrupt handler to finish
    }

    int_flag = 0;

    return 0;
}

int32_t write_rtc(int32_t fd, const int32_t* buf, int32_t nbytes)
{
    int32_t write_freq;

    if(nbytes != 4 || buf == NULL)
        return -1;
    else
        write_freq = *(buf);

    set_freq(write_freq);

    return nbytes;
}

int32_t close_rtc(int32_t fd)
{
    set_freq(2);

    return 0;
}

void set_freq(int32_t freq)
{
    uint8_t new_freq, old_freq;

    outb(STAT_REG_A, RTC_PORT);
    old_freq = inb(RW_CMOS);

    switch(freq)
    {
        case 2:
            new_freq = 0x0F;
            break;

        case 4:
            new_freq = 0x0E;
            break;

        case 8:
            new_freq = 0x0D;
            break;

        case 16:
            new_freq = 0x0C;
            break;

        case 32:
            new_freq = 0x0B;
            break;

        case 64:
            new_freq = 0x0A;
            break;

        case 128:
            new_freq = 0x09;
            break;

        case 256:
            new_freq = 0x08;
            break;

        case 512:
            new_freq = 0x07;
            break;

        case 1024:
            new_freq = 0x06;
            break;

        default:
            return;
    }

    outb(STAT_REG_A, RTC_PORT);
    outb((old_freq & 0xF0) | new_freq, RW_CMOS);
}


void test_rtc(){
    int i,key;
    int freq[10] = {2,4,8,16,32,64,128,256,512,1024};
    disp_handler = 1;
    clear();
    resetCursor();
    for(i = 0; i < 10; i++){
        key = get_buf_idx();
        write_rtc(0,&freq[i],4);
        while(key == get_buf_idx());
        bksp_handler();
    }
    disp_handler = 0;
}
