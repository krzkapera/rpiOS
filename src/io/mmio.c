#include "mmio.h"

void mmio_write(long reg, uint32_t val) {
	*(volatile uint32_t*)reg = val;
}

uint32_t mmio_read(long reg) {
	return *(volatile uint32_t*)reg;
}

void gpio_function(uint32_t pin_number, uint32_t function) {
	uint32_t field_mask = (1 << 3) - 1;

	uint32_t reg = GPFSELn + (pin_number / 10) * 4;
	uint32_t shift = (pin_number % 10) * 3;

	uint32_t value = mmio_read(reg);
	value &= ~(field_mask << shift);
	value |= function << shift;
	mmio_write(reg, value);
}
