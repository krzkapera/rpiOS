#ifndef __SYS_TIMER_H__
#define __SYS_TIMER_H__

#include "../io/mmio.h"

#define INTERRUPT_INTERVAL 1000000

enum {
	TIMER_BASE = PERIPHERAL_BASE + 0x3000,
	TIMER_CS = TIMER_BASE,
	TIMER_CLO = TIMER_BASE + 0x04,
	TIMER_CHI = TIMER_BASE + 0x08,
	TIMER_C0 = TIMER_BASE + 0x0C,
	TIMER_C1 = TIMER_BASE + 0x10,
	TIMER_C3 = TIMER_BASE + 0x18,

	TIMER_0 = 1,
	TIMER_1 = 1 << 1,
	TIMER_3 = 1 << 3,
};

void wait(uint32_t microseconds);
uint32_t get_time();
void init_timer();

#endif // __SYS_TIMER_H__
