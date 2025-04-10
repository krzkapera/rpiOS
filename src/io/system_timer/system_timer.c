#include "system_timer.h"

void wait_v0(uint32_t microseconds) {
	uint32_t start_time = mmio_read(TIMER_CLO);
	while ((mmio_read(TIMER_CLO) - start_time) < microseconds)
		;
}

void wait(uint32_t microseconds) {
	mmio_write(TIMER_C0, mmio_read(TIMER_CLO) + microseconds);
	mmio_write(TIMER_CS, mmio_read(TIMER_CS) | 1);

	while ((mmio_read(TIMER_CS) & 1) == 0)
		;
}
