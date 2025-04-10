#include "io/gpio/gpio.h"
#include "io/system_timer/system_timer.h"
#include "io/uart/printf.h"
#include <stdint.h>

void main() {
	uart_init();
	printf("Compilation time: " __DATE__ " " __TIME__ "\r\n");

	// while (1)
	// 	uart_update();

	mmio_write(GPFSEL4, mmio_read(GPFSEL4) | 0x40);

	while (1) {
		mmio_write(GPSET1, 1 << 10);
		wait(2e6);
		mmio_write(GPCLR1, 1 << 10);
		wait(2e6);
	}
}
