#include "mmio.h"

void mmio_write(long reg, uint32_t val) {
	*(volatile uint32_t*)reg = val;
}

uint32_t mmio_read(long reg) {
	return *(volatile uint32_t*)reg;
}
