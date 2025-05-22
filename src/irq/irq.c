#include "irq.h"
#include "../io/system_timer/system_timer.h"
#include "../io/uart/printf.h"
#include "../io/uart/uart.h"

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

uint32_t ALLOWED_INTERRUPTS = IRQ_AUX | IRQ_TIMER_1;

void enable_interrupt_controller() {
	mmio_write(IRQ0_SET_EN_0, ALLOWED_INTERRUPTS);
}

void handle_irq() {
	uint32_t irq = mmio_read(IRQ0_PENDING0);

	if (irq & IRQ_AUX) {
		irq &= ~IRQ_AUX;
		while ((mmio_read(AUX_MU_IIR_REG) & 4)) {
			printf("Mini-UART Recv: %c\n", getchar());
		}
	}

	if (irq & IRQ_TIMER_1) {
		irq &= ~IRQ_TIMER_1;

		puts("timer\n");
		mmio_write(TIMER_CS, mmio_read(TIMER_CS) | TIMER_1);
	}
}
