#include "power.h"

void restart() {
	uint32_t r = mmio_read(PM_RSTS) & ~0xfffffaaa;

	mmio_write(PM_RSTS, PM_WDOG_MAGIC | r);
	mmio_write(PM_WDOG, PM_WDOG_MAGIC | 10);
	mmio_write(PM_RSTC, PM_WDOG_MAGIC | PM_RSTC_FULLRST);
}
