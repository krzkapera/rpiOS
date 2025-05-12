#include "power.h"

void power_off() {
	// r = *PM_RSTS;
	// r &= ~0xfffffaaa;
	// r |= 0x555; // partition 63 used to indicate halt
	// *PM_RSTS = PM_WDOG_MAGIC | r;
	// *PM_WDOG = PM_WDOG_MAGIC | 10;
	// *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}

void restart() {
	const int PM_RSTC = 0xFE10001c;
	const int PM_WDOG = 0xFE100024;
	const int PM_PASSWORD = 0x5a000000;
	const int PM_RSTC_WRCFG_FULL_RESET = 0x00000020;

	mmio_write(PM_WDOG, PM_PASSWORD | 1); // timeout = 1/16th of a second? (whatever)
	mmio_write(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
	while (1)
		;
}
