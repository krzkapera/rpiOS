#ifndef __MMIO_H__
#define __MMIO_H__

#include <stdint.h>

enum {
	PERIPHERAL_BASE = 0xFE000000,
	GPIO_BASE = PERIPHERAL_BASE + 0x200000,

	GPFSELn = GPIO_BASE,

	GPSET0 = GPIO_BASE + 0x1C,
	GPSET1 = GPIO_BASE + 0x20,

	GPCLR0 = GPIO_BASE + 0x28,
	GPCLR1 = GPIO_BASE + 0x2c,

	GPIO_FUNCTION_OUTPUT = 1,
	GPIO_FUNCTION_ALT5 = 2
};

void gpio_function(uint32_t pin_number, uint32_t function);

void mmio_write(long reg, uint32_t val);
uint32_t mmio_read(long reg);

#endif // __MMIO_H__
