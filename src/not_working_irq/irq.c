#include "irq.h"
#include "../io/uart/uart.h"

const char entry_error_messages[16][32] = {
	"SYNC_INVALID_EL1t",   "IRQ_INVALID_EL1t",	 "FIQ_INVALID_EL1t",   "SERROR_INVALID_EL1t",
	"SYNC_INVALID_EL1h",   "IRQ_INVALID_EL1h",	 "FIQ_INVALID_EL1h",   "SERROR_INVALID_EL1h",
	"SYNC_INVALID_EL0_64", "IRQ_INVALID_EL0_64", "FIQ_INVALID_EL0_64", "SERROR_INVALID_EL0_64",
	"SYNC_INVALID_EL0_32", "IRQ_INVALID_EL0_32", "FIQ_INVALID_EL0_32", "SERROR_INVALID_EL0_32"};

void show_invalid_entry_message(uint32_t type, uint64_t esr, uint64_t address) {
	uart_write_text("ERROR CAUGHT: ");
	uart_write_text(entry_error_messages[type]);
	uart_write_text(", ERS: TODO, Address: TODO \n");
}

void enable_core0_interrupt_controller_AUX() {
	mmio_write(IRQ0_SET_EN_0, IRQ_ON);
}

// extern int counter;

void handle_irq() {
	uint32_t irq = mmio_read(IRQ0_PENDING0);

	while (irq) {
		if (irq & IRQ_ON) {
			irq &= ~IRQ_ON;
			while ((mmio_read(AUX_MU_IIR_REG) & 4) == 4) {
				// counter = 77;

				uart_write_text("Mini-UART Recv:\r\n");
				// uart_write_byte(uart_read_byte());
				// uart_write_text("\n");
			}
		}
	}
}
