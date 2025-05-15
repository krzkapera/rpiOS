#include "io/gpio/gpio.h"
#include "io/system_timer/system_timer.h"
#include "io/uart/printf.h"
// #include "irq/irq.h"
#include "power/power.h"
#include <stdarg.h>
#include <stdint.h>

extern uint32_t get_el();

int test(const char* fmt, ...) {
	int state = 5;
	va_list args;

	va_start(args, fmt);
	va_end(args);
	return 0;
}

void main() {
	uart_write_text("\r\nCompilation time: " __DATE__ " " __TIME__ "\r\n");
	uart_write_byte(get_el() + '0');
	uart_write_text("\r\nHMM\r\n");
	test(0);
	uart_write_text("XD\r\n");

	gpio_function(42, GPIO_FUNCTION_OUTPUT);

	for (int i = 0; i < 5; i++) {
		mmio_write(GPSET1, 1 << 10);
		wait(2e6);
		mmio_write(GPCLR1, 1 << 10);
		wait(2e6);
	}

	while (1) {
	}
}
