#ifndef RTC_H
#define RTC_H

#include "lib.h"
#include "types.h"
#include "i8259.h"


#define RTC_PORT  0x70
#define RW_CMOS 0x71
#define STAT_REG_A  0x8A
#define STAT_REG_B  0x8B
#define STAT_REG_C  0x8C
#define ENABLE_BIT_SIX  0x40
#define RTC_IRQ_NUM 8
#define RATE 5

extern void rtc_init(void);
extern void rtc_handler();

#endif