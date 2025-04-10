#include "uart.h"
#include "../gpio/gpio.h"

void uart_init() {
	mmio_write(AUX_ENABLES, 1); // enable UART1
	mmio_write(AUX_MU_IER_REG, 0);
	mmio_write(AUX_MU_CNTL_REG, 0);
	mmio_write(AUX_MU_LCR_REG, 3); // 8 bits
	mmio_write(AUX_MU_MCR_REG, 0);
	mmio_write(AUX_MU_IER_REG, 0);
	mmio_write(AUX_MU_IIR_REG, 0xC6); // disable interrupts
	mmio_write(AUX_MU_BAUD_REG, AUX_MU_BAUD(115200));
	gpio_useAsAlt5(14);
	gpio_useAsAlt5(15);
	mmio_write(AUX_MU_CNTL_REG, 3); // enable RX/TX
}

uint32_t uart_is_read_byte_ready() {
	return mmio_read(AUX_MU_LSR_REG) & 0x01;
}

uint32_t uart_is_write_byte_ready() {
	return mmio_read(AUX_MU_LSR_REG) & 0x20;
}

uint8_t uart_read_byte() {
	while (!uart_is_read_byte_ready())
		;
	return (uint8_t)mmio_read(AUX_MU_IO_REG);
}

void uart_write_byte(uint8_t ch) {
	while (!uart_is_write_byte_ready())
		;
	mmio_write(AUX_MU_IO_REG, (uint32_t)ch);
}

void uart_write_text(char* buffer) {
	while (*buffer) {
		uart_write_byte(*buffer++);
	}
}

void uart_update() {
	if (uart_is_read_byte_ready()) {
		uint8_t ch = uart_read_byte();
		uart_write_byte(ch);
	}
}
