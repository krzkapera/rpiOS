#include "io.h"
#include "printf.h"
#include <stdint.h>

void write32(void* dst, uint32_t val) {
	uint32_t* dst_u = (uint32_t*)dst;
	*dst_u = val;

	return;
}

uint32_t read32(void* src) {
	uint32_t* src_u = (uint32_t*)src;
	return *src_u;
}

#define GPIO_BASE 0xFE200000

#define GPIO_GPFSEL4 (void*)(GPIO_BASE + 0x10)
#define GPIO_GPSET1 (void*)(GPIO_BASE + 0x20)
#define GPIO_GPCLR1 (void*)(GPIO_BASE + 0x2c)

#define TIMER_BASE 0xFE003000

#define TIMER_CS (volatile uint32_t*)TIMER_BASE
#define TIMER_CLO (volatile uint32_t*)(TIMER_BASE + 0x04)
#define TIMER_CHI (volatile uint32_t*)(TIMER_BASE + 0x08)
#define TIMER_C0 (volatile uint32_t*)(TIMER_BASE + 0x0C)

void wait_v0(uint32_t microseconds) {
	uint32_t start_time = *TIMER_CLO;
	while ((*TIMER_CLO - start_time) < microseconds) {
	}
}

void wait(uint32_t microseconds) {
	*TIMER_C0 = *TIMER_CLO + microseconds;
	*TIMER_CS |= 1;

	while ((*TIMER_CS & 1) == 0) {
	}
}

void main() {
	uart_init();
	uart_writeText("Compilation time: " __DATE__ " " __TIME__ "\n");

	while (1)
		uart_update();
	// printf("Helli worldzddix %d!\r\n", 7);

	// uint32_t gpfsel4 = read32(GPIO_GPFSEL4);
	// gpfsel4 |= 0x40;
	// write32(GPIO_GPFSEL4, gpfsel4);

	while (1) {
		write32(GPIO_GPSET1, 1 << 10);
		wait(3e6);
		write32(GPIO_GPCLR1, 1 << 10);
		wait(3e6);
	}
}
