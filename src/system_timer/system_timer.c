#include "system_timer.h"

void wait(uint32_t microseconds) {
	uint32_t start_time = mmio_read(TIMER_CLO);
	while ((mmio_read(TIMER_CLO) - start_time) < microseconds) {
		asm volatile("nop");
	}
}

uint32_t get_time() {
	return mmio_read(TIMER_CLO);
}

void init_timer() {
	mmio_write(TIMER_C1, mmio_read(TIMER_CLO) + INTERRUPT_INTERVAL);
}
