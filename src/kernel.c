#include "fork/fork.h"
#include "io/mmio.h"
#include "io/uart/printf.h"
#include "irq/irq.h"
#include "power/power.h"
#include "scheduler/scheduler.h"
#include "sd/fatfs/ff.h"
#include "syscalls/syscalls.h"
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

void user_process1(char* array) {
	char buf[3] = {0, '\n', 0};
	while (1) {
		for (int i = 0; i < 5; i++) {
			buf[0] = array[i];
			call_sys_write(buf);
			wait(1e6);
		}
	}
}

void user_process() {
	char buf[30] = {0};
	memcpy(buf, "User process started\n", 22);
	call_sys_write(buf);
	uint64_t stack = call_sys_malloc();
	if (stack < 0) {
		printf("Error while allocating stack for process 1\n");
		return;
	}
	int err = call_sys_clone((uint64_t)&user_process1, (uint64_t)"12345", stack);
	if (err < 0) {
		printf("Error while clonning process 1\n");
		return;
	}
	stack = call_sys_malloc();
	if (stack < 0) {
		printf("Error while allocating stack for process 2\n");
		return;
	}
	err = call_sys_clone((uint64_t)&user_process1, (uint64_t)"abcd", stack);
	if (err < 0) {
		printf("Error while clonning process 2\n");
		return;
	}
	call_sys_exit();
}

void kernel_process() {
	printf("Kernel process started. EL %d\r\n", get_el());
	int err = move_to_user_mode((uint64_t)&user_process);
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

	int res = copy_process(PF_KTHREAD, (uint64_t)&kernel_process, (uint64_t)"12345", 0);
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
