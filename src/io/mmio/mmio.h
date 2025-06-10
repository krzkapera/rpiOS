#ifndef __MMIO_H__
#define __MMIO_H__

#include <stdint.h>

enum { PERIPHERAL_BASE = 0xFE000000 };

void mmio_write(long reg, uint32_t val);
uint32_t mmio_read(long reg);

#endif // __MMIO_H__
