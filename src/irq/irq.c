#include "irq.h"
#include "../io/system_timer/system_timer.h"
#include "../io/uart/printf.h"
#include "../io/uart/uart.h"
#include "../system_loader/system_loader.h"

const char entry_error_messages[16][32] = {
	"SYNC_INVALID_EL1t",   "IRQ_INVALID_EL1t",	 "FIQ_INVALID_EL1t",   "SERROR_INVALID_EL1t",
	"SYNC_INVALID_EL1h",   "IRQ_INVALID_EL1h",	 "FIQ_INVALID_EL1h",   "SERROR_INVALID_EL1h",
	"SYNC_INVALID_EL0_64", "IRQ_INVALID_EL0_64", "FIQ_INVALID_EL0_64", "SERROR_INVALID_EL0_64",
	"SYNC_INVALID_EL0_32", "IRQ_INVALID_EL0_32", "FIQ_INVALID_EL0_32", "SERROR_INVALID_EL0_32"};

void show_invalid_entry_message(uint32_t type, uint64_t esr, uint64_t address) {
	puts("ERROR CAUGHT: ");
	puts(entry_error_messages[type]);
	puts(", ERS: TODO, Address: TODO \n");
}

void enable_interrupts() {
	irq_init_vectors();
	mmio_write(IRQ0_SET_EN_0, ALLOWED_INTERRUPTS);
	irq_enable();
}

uint32_t ALLOWED_INTERRUPTS = IRQ_AUX | IRQ_TIMER_1;

void handle_irq() {
	uint32_t irq = mmio_read(IRQ0_PENDING0);

	if (irq & IRQ_AUX) {
		irq &= ~IRQ_AUX;
		read_data();
	}

	if (irq & IRQ_TIMER_1) {
		irq &= ~IRQ_TIMER_1;

		puts("timer\n");
		mmio_write(TIMER_CS, mmio_read(TIMER_CS) | TIMER_1);
	}
}
