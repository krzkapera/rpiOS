#include "io/gpio/gpio.h"
// #include "io/power/power.h"
#include "io/system_timer/system_timer.h"
#include "io/uart/printf.h"
#include <stdint.h>

void main() {
	uart_init();
	printf("Compilation time: " __DATE__ " " __TIME__ "\r\n");

	// while (1)
	// 	uart_update();

	gpio_function(42, GPIO_FUNCTION_OUTPUT);

	while (1) {
		mmio_write(GPSET1, 1 << 10);
		wait(2e6);
		mmio_write(GPCLR1, 1 << 10);
		wait(2e6);
	}

	// restart();
}
