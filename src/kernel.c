#include "io/gpio/gpio.h"
#include "io/system_timer/system_timer.h"
#include "io/uart/printf.h"
// #include "irq/irq.h"
#include "power/power.h"
#include <stdarg.h>
#include <stdint.h>

extern uint32_t get_el();

void main() {
	uart_init();
	printf("Compilation time: " __DATE__ " " __TIME__ " EL: %d\r\n", get_el() + '0');

	// irq_init_vectors();
	// enable_core0_interrupt_controller_AUX();
	// irq_enable();

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
