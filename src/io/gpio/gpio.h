#ifndef __GPIO_H__
#define __GPIO_H__

#include "../mmio/mmio.h"
#include <stdint.h>

enum {
	GPIO_BASE = PERIPHERAL_BASE + 0x200000,

	GPFSEL0 = GPIO_BASE,
	GPSET0 = GPIO_BASE + 0x1C,

	GPCLR0 = GPIO_BASE + 0x28,
	GPIO_PUP_PDN_CNTRL_REG0 = GPIO_BASE + 0xE4,

	GPFSEL4 = GPIO_BASE + 0x10,
	GPSET1 = GPIO_BASE + 0x20,
	GPCLR1 = GPIO_BASE + 0x2c
};

enum { GPIO_MAX_PIN = 57, GPIO_FUNCTION_OUT = 1, GPIO_FUNCTION_ALT5 = 2, GPIO_FUNCTION_ALT3 = 7 };

enum { Pull_None = 0, Pull_Down = 2, Pull_Up = 1 };

uint32_t gpio_call(uint32_t pin_number, uint32_t value, uint32_t base, uint32_t field_size);
uint32_t gpio_set(uint32_t pin_number, uint32_t value);
uint32_t gpio_clear(uint32_t pin_number, uint32_t value);
uint32_t gpio_pull(uint32_t pin_number, uint32_t value);
uint32_t gpio_function(uint32_t pin_number, uint32_t value);
void gpio_useAsAlt5(uint32_t pin_number);

#endif // __GPIO_H__
