#include "irq.h"
#include "../io/uart/printf.h"
#include "../io/uart/uart.h"
#include "../scheduler/scheduler.h"
#include "../system_loader/system_loader.h"
#include "../system_timer/system_timer.h"

const char entry_error_messages[18][32] = {
	"SYNC_INVALID_EL1t",   "IRQ_INVALID_EL1t",	 "FIQ_INVALID_EL1t",   "ERROR_INVALID_EL1t",
	"SYNC_INVALID_EL1h",   "IRQ_INVALID_EL1h",	 "FIQ_INVALID_EL1h",   "ERROR_INVALID_EL1h",
	"SYNC_INVALID_EL0_64", "IRQ_INVALID_EL0_64", "FIQ_INVALID_EL0_64", "ERROR_INVALID_EL0_64",
	"SYNC_INVALID_EL0_32", "IRQ_INVALID_EL0_32", "FIQ_INVALID_EL0_32", "ERROR_INVALID_EL0_32",
	"SYNC_ERROR",		   "SYSCALL_ERROR"};

void show_invalid_entry_message(uint32_t type, uint64_t esr, uint64_t address) {
	printf("ERROR CAUGHT: %s, ESR: %d\n", entry_error_messages[type], esr);
}

void enable_interrupts() {
	irq_init_vectors();
	mmio_write(IRQ0_SET_EN_0, ALLOWED_INTERRUPTS);
	irq_enable();
}

void disable_timer() {
	mmio_write(IRQ0_CLR_EN_0, IRQ_TIMER_1);
}

uint32_t ALLOWED_INTERRUPTS = IRQ_AUX | IRQ_TIMER_1;

void handle_irq() {
	uint32_t irq = mmio_read(IRQ0_PENDING0);

	if (irq & IRQ_AUX) {
		disable_timer();
		read_data();
	}

	if (irq & IRQ_TIMER_1) {
		mmio_write(TIMER_CS, mmio_read(TIMER_CS) | TIMER_1);
		mmio_write(TIMER_C1, mmio_read(TIMER_CLO) + INTERRUPT_INTERVAL);

		timer_tick();
	}
}
