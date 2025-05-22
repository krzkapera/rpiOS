#include "io/gpio/gpio.h"
#include "io/system_timer/system_timer.h"
#include "io/uart/printf.h"
#include "irq/irq.h"
#include "power/power.h"
#include "sd/fatfs/ff.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

extern uint32_t get_el();
extern unsigned char _end;

void main() {
	uart_init();
	puts("EL: ");
	putchar(get_el() + '0');
	printf("\nCompilation time: " __DATE__ " " __TIME__ "\n");

	irq_init_vectors();
	enable_interrupt_controller();
	irq_enable();

	gpio_function(42, GPIO_FUNCTION_OUTPUT);

	mmio_write(TIMER_C1, mmio_read(TIMER_CLO) + 10 * 1000000);

	for (int i = 0; i < 5; i++) {
		mmio_write(GPSET1, 1 << 10);
		wait(2e6);
		mmio_write(GPCLR1, 1 << 10);
		wait(2e6);
	}

	while (1) {
	}
}
