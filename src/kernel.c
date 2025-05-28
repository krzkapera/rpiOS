#include "io/gpio/gpio.h"
#include "io/uart/printf.h"
#include "irq/irq.h"
#include "power/power.h"
#include "scheduler/scheduler.h"
#include "sd/fatfs/ff.h"
#include "system_timer/system_timer.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

extern uint32_t get_el();

void blink_led();

void process(char* array) {
	while (1) {
		puts("process\n");
		wait(1e6);
	}
}

void main() {
	uart_init();

	puts("EL: ");
	putchar(get_el() + '0');

	printf("\nCompilation time: " __DATE__ " " __TIME__ "\n");

	enable_interrupts();
	init_timer();

	int res = copy_process((unsigned long)&process, (unsigned long)"12345");
	if (res != 0) {
		printf("error while starting process 1");
	}
	// res = copy_process((unsigned long)&process, (unsigned long)"abcde");
	// if (res != 0) {
	// 	printf("error while starting process 2");
	// }

	// puts("przeszlo\n");

	while (1) {
		// schedule();
		puts("main\n");
		wait(1e6);
	}
}

void blink_led() {
	gpio_function(42, GPIO_FUNCTION_OUTPUT);
	mmio_write(GPSET1, 1 << 10);
	wait(2e6);
	mmio_write(GPCLR1, 1 << 10);
	wait(2e6);
}
