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
extern uint64_t user_begin;
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
	int pid = call_sys_fork();
	if (pid < 0) {
		call_sys_write("Error during fork\n\r");
		call_sys_exit();
		return;
	}
	if (pid == 0) {
		loop("abcde");
	} else {
		loop("12345");
	}
}

void kernel_process() {
	printf("Kernel process started. EL %d\r\n", get_el());
	uint64_t begin = (uint64_t)&user_begin;
	uint64_t end = (uint64_t)&user_end;
	uint64_t process = (uint64_t)&user_process;
	int err = move_to_user_mode(begin, end - begin, process - begin);
	if (err < 0) {
		printf("Error while moving process to user mode\n\r");
	}
}

void main() {
	uart_init();

	puts("EL: ");
	putchar(get_el() + '0');

	printf("\nCompilation time: " __DATE__ " " __TIME__ "\n");

	enable_interrupts();
	init_timer();

	int res = copy_process(PF_KTHREAD, (uint64_t)&kernel_process, 0);
	if (res < 0) {
		printf("error while starting kernel process\n");
	}

	while (1);
}

void blink_led() {
	gpio_function(42, GPIO_FUNCTION_OUTPUT);
	mmio_write(GPSET1, 1 << 10);
	wait(2e6);
	mmio_write(GPCLR1, 1 << 10);
	wait(2e6);
}
