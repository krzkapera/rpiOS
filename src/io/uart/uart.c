#include "uart.h"
#include "../gpio/gpio.h"

void uart_init() {
	gpio_function(14, GPIO_FUNCTION_ALT5);
	gpio_function(15, GPIO_FUNCTION_ALT5);

	mmio_write(AUX_ENABLES, 1);
	mmio_write(AUX_MU_IER_REG, 2);
	mmio_write(AUX_MU_CNTL_REG, 0);
	mmio_write(AUX_MU_LCR_REG, 3);
	mmio_write(AUX_MU_MCR_REG, 0);
	mmio_write(AUX_MU_IER_REG, 2);
	mmio_write(AUX_MU_CNTL_REG, 3);
	mmio_write(AUX_MU_IIR_REG, 0xC6);
	mmio_write(AUX_MU_BAUD_REG, AUX_MU_BAUD(115200));
	mmio_write(AUX_MU_CNTL_REG, 3);
}

uint32_t uart_is_read_byte_ready() {
	return mmio_read(AUX_MU_LSR_REG) & 0x01;
}

uint32_t uart_is_write_byte_ready() {
	return mmio_read(AUX_MU_LSR_REG) & 0x20;
}

uint8_t getchar() {
	while (!uart_is_read_byte_ready())
		;
	return (uint8_t)mmio_read(AUX_MU_IO_REG);
}

void putchar(uint8_t ch) {
	if (ch == '\n') {
		putchar('\r');
	}
	while (!uart_is_write_byte_ready())
		;
	mmio_write(AUX_MU_IO_REG, (uint32_t)ch);
}

void puts(const char* buffer) {
	while (*buffer) {
		putchar(*buffer++);
	}
}

void uart_update() {
	if (uart_is_read_byte_ready()) {
		uint8_t ch = getchar();
		putchar(ch);
	}
}

void puthex(unsigned int d) {
	unsigned int n;
	int c;
	for (c = 28; c >= 0; c -= 4) {
		// get highest tetrad
		n = (d >> c) & 0xF;
		// 0-9 => '0'-'9', 10-15 => 'A'-'F'
		n += n > 9 ? 0x37 : 0x30;
		putchar(n);
	}
}

void uart_dump(void* ptr) {
	unsigned long a, b, d;
	unsigned char c;
	for (a = (unsigned long)ptr; a < (unsigned long)ptr + 512; a += 16) {
		puthex(a);
		puts(": ");
		for (b = 0; b < 16; b++) {
			c = *((unsigned char*)(a + b));
			d = (unsigned int)c;
			d >>= 4;
			d &= 0xF;
			d += d > 9 ? 0x37 : 0x30;
			putchar(d);
			d = (unsigned int)c;
			d &= 0xF;
			d += d > 9 ? 0x37 : 0x30;
			putchar(d);
			putchar(' ');
			if (b % 4 == 3) {
				putchar(' ');
			}
		}
		for (b = 0; b < 16; b++) {
			c = *((unsigned char*)(a + b));
			putchar(c < 32 || c >= 127 ? '.' : c);
		}
		puts("\n");
	}
}
