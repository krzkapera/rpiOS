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

void uart_write_text(const char* buffer) {
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
