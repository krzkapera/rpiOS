#include "uart.h"
#include "../gpio/gpio.h"

#define AUX_MU_BAUD(baud) ((AUX_UART_CLOCK / (baud * 8)) - 1)

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

unsigned int uart_isReadByteReady() {
	return mmio_read(AUX_MU_LSR_REG) & 0x01;
}

unsigned int uart_isWriteByteReady() {
	return mmio_read(AUX_MU_LSR_REG) & 0x20;
}

unsigned char uart_readByte() {
	while (!uart_isReadByteReady())
		;
	return (unsigned char)mmio_read(AUX_MU_IO_REG);
}

void uart_writeByte(unsigned char ch) {
	while (!uart_isWriteByteReady())
		;
	mmio_write(AUX_MU_IO_REG, (unsigned int)ch);
}

void uart_writeText(char* buffer) {
	while (*buffer) {
		uart_writeByte(*buffer++);
	}
}

void uart_update() {
	if (uart_isReadByteReady()) {
		unsigned char ch = uart_readByte();
		uart_writeByte(ch);
	}
}