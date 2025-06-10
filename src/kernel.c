#include "io/gpio/gpio.h"
#include "io/uart/printf.h"
#include "irq/irq.h"
#include "power/power.h"
#include "scheduler/scheduler.h"
#include "sd/fatfs/ff.h"
#include "sys/fork.h"
#include "sys/user_sys.h"
#include "system_timer/system_timer.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

extern uint32_t get_el();
extern uint64_t user_start;
extern uint64_t user_end;

void blink_led();

void process(char* array) {
	while (1) {
		puts("process\n");
		wait(1e6);
	}
}

void loop(char* str) {
	char buf[2] = {""};
	while (1) {
		for (int i = 0; i < 5; i++) {
			buf[0] = str[i];
			call_sys_write(buf);
			user_delay(1000000);
		}
	}
}

void user_process() {
	call_sys_write("User process\n\r");
	loop("abcde");
	// int pid = call_sys_fork();
	// if (pid < 0) {
	// 	call_sys_write("Error during fork\n\r");
	// 	call_sys_exit();
	// 	return;
	// }
	// if (pid == 0) {
	// 	loop("abcde");
	// } else {
	// 	loop("12345");
	// }
}

void kernel_process() {
	printf("pid 1 started in EL %c\n", get_el() + '0');

	uint64_t user_size = ((uint64_t)(&user_end)) - ((uint64_t)(&user_start));

	int err = prepare_move_to_user((uint64_t)&user_start, user_size,
								   ((uint64_t)user_process) - (uint64_t)&user_start);
	if (err < 0)
		puts("Failed to move to user mode!\n");
	while (1);
}

void main() {
	uart_init();

	puts("EL: ");
	putchar(get_el() + '0');

	printf("\nCompilation time: " __DATE__ " " __TIME__ "\n");

	enable_interrupts();
	init_timer();

	int res = copy_process(KTHREAD, (uint64_t)&kernel_process, 0);
	if (res <= 0) {
		puts("fork error\n");
	}

	while (1) {
		// schedule();
	}
}

void blink_led() {
	gpio_function(42, GPIO_FUNCTION_OUTPUT);
	mmio_write(GPSET1, 1 << 10);
	wait(2e6);
	mmio_write(GPCLR1, 1 << 10);
	wait(2e6);
}
