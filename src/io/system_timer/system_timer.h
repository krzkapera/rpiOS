#ifndef __SYS_TIMER_H__
#define __SYS_TIMER_H__

#include "../mmio/mmio.h"

enum {
	TIMER_BASE = PERIPHERAL_BASE + 0x3000,
	TIMER_CS = TIMER_BASE,
	TIMER_CLO = TIMER_BASE + 0x04,
	TIMER_CHI = TIMER_BASE + 0x08,
	TIMER_C0 = TIMER_BASE + 0x0C,
	TIMER_C1 = TIMER_BASE + 0x10,
};

void wait(uint32_t microseconds);

#endif // __SYS_TIMER_H__
