#include "system_timer.h"

void wait(uint32_t microseconds) {
	uint32_t start_time = mmio_read(TIMER_CLO);
	while ((mmio_read(TIMER_CLO) - start_time) < microseconds)
		;
}
