#ifndef __GPIO_H__
#define __GPIO_H__

#include "../mmio/mmio.h"
#include <stdint.h>

enum {
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

#endif // __GPIO_H__
