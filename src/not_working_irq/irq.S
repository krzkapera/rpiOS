.globl irq_init_vectors
irq_init_vectors:
    adr x0, vectors
    msr vbar_el2, x0
    ret

.globl irq_enable
irq_enable:
    msr daifclr, #2 /* clear the 'i' mask */
    ret

.globl irq_disable
irq_disable:
    msr daifset, #2 /* set the 'i' mask */
    ret
