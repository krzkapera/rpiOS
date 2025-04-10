#ifndef __UART_H__
#define __UART_H__

#include "../gpio/gpio.h"

enum {
	AUX_BASE = GPIO_BASE + 0x15000,
	AUX_IRQ = AUX_BASE,
	AUX_ENABLES = AUX_BASE + 4,
	AUX_MU_IO_REG = AUX_BASE + 64,
	AUX_MU_IER_REG = AUX_BASE + 68,
	AUX_MU_IIR_REG = AUX_BASE + 72,
	AUX_MU_LCR_REG = AUX_BASE + 76,
	AUX_MU_MCR_REG = AUX_BASE + 80,
	AUX_MU_LSR_REG = AUX_BASE + 84,
	AUX_MU_MSR_REG = AUX_BASE + 88,
	AUX_MU_SCRATCH = AUX_BASE + 92,
	AUX_MU_CNTL_REG = AUX_BASE + 96,
	AUX_MU_STAT_REG = AUX_BASE + 100,
	AUX_MU_BAUD_REG = AUX_BASE + 104,
	AUX_UART_CLOCK = 500000000,
	UART_MAX_QUEUE = 16 * 1024
};

#define AUX_MU_BAUD(baud) ((AUX_UART_CLOCK / (baud * 8)) - 1)

void uart_init();
void uart_write_text(char* buffer);
void uart_write_byte(uint8_t ch);
void uart_update();

#endif // __UART_H__
