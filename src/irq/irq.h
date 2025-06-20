#ifndef __IRQ_H__
#define __IRQ_H__

#include "../io/mmio.h"

void irq_init_vectors();
void irq_enable();
void irq_disable();

void enable_interrupts();

enum {
	IRQ_AUX = (1 << 29),
	IRQ_TIMER_0 = 1 << 0,
	IRQ_TIMER_1 = 1 << 1,
	IRQ_TIMER_3 = 1 << 3,

	ARMC_BASE = PERIPHERAL_BASE + 0xb000,

	IRQ0_PENDING0 = ARMC_BASE + 0x200,
	IRQ0_PENDING1 = ARMC_BASE + 0x204,
	IRQ0_PENDING2 = ARMC_BASE + 0x208,

	IRQ0_SET_EN_0 = ARMC_BASE + 0x210,
	IRQ0_SET_EN_1 = ARMC_BASE + 0x214,
	IRQ0_SET_EN_2 = ARMC_BASE + 0x218,

	IRQ0_CLR_EN_0 = ARMC_BASE + 0x220,
	IRQ0_CLR_EN_1 = ARMC_BASE + 0x224,
	IRQ0_CLR_EN_2 = ARMC_BASE + 0x228,
};

extern uint32_t ALLOWED_INTERRUPTS;

#endif // __IRQ_H__
