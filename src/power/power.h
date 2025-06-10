#ifndef __POWER_H__
#define __POWER_H__

#include "../io/mmio.h"

enum {
	PM_RSTC = PERIPHERAL_BASE + 0x0010001c,
	PM_RSTS = PERIPHERAL_BASE + 0x00100020,
	PM_WDOG = PERIPHERAL_BASE + 0x00100024,

	PM_WDOG_MAGIC = 0x5a000000,
	PM_RSTC_FULLRST = 0x00000020
};

void restart();

#endif // __POWER_H__
