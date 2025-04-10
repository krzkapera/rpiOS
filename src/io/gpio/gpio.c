#include "gpio.h"
#include <stdint.h>

uint32_t gpio_call(uint32_t pin_number, uint32_t value, uint32_t base, uint32_t field_size) {
	uint32_t field_mask = (1 << field_size) - 1;

	if (pin_number > GPIO_MAX_PIN)
		return 0;
	if (value > field_mask)
		return 0;

	uint32_t num_fields = 32 / field_size;
	uint32_t reg = base + ((pin_number / num_fields) * 4);
	uint32_t shift = (pin_number % num_fields) * field_size;

	uint32_t curval = mmio_read(reg);
	curval &= ~(field_mask << shift);
	curval |= value << shift;
	mmio_write(reg, curval);

	return 1;
}

uint32_t gpio_set(uint32_t pin_number, uint32_t value) {
	return gpio_call(pin_number, value, GPSET0, 1);
}

uint32_t gpio_clear(uint32_t pin_number, uint32_t value) {
	return gpio_call(pin_number, value, GPCLR0, 1);
}

uint32_t gpio_pull(uint32_t pin_number, uint32_t value) {
	return gpio_call(pin_number, value, GPIO_PUP_PDN_CNTRL_REG0, 2);
}

uint32_t gpio_function(uint32_t pin_number, uint32_t value) {
	return gpio_call(pin_number, value, GPFSEL0, 3);
}

void gpio_useAsAlt5(uint32_t pin_number) {
	gpio_pull(pin_number, Pull_None);
	gpio_function(pin_number, GPIO_FUNCTION_ALT5);
}
