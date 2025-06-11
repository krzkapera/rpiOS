#include "mem/mem.h"
#include "irq/sysregs.h"

.section ".text.boot"

.global _start

_start:
    // Check processor ID is zero (executing on main core), else hang
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 1f
    // We're not on the main core, so hang in an infinite wait loop
    wfe
    b       .
1:  // We're on the main core!

    // Set stack to start below our code
    mov     sp, #LOW_MEMORY

    // Clean the BSS section
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

4:  nop

    // enable floating point registers, used in printf
    mrs     x0, cptr_el3
    bic     x0, x0, #(1 << 31)
    bic     x0, x0, #(1 << 10)
    msr     cptr_el3, x0

    // Enable FP/SIMD at EL1
    mov     x0, #3 << 20
    msr     cpacr_el1, x0

    // switch to EL 1
    mov     x0, #LOW_MEMORY
    msr     sp_el1, x0
    
    mrs     x0, hcr_el2
    orr     x0, x0, #HCR_VALUE
    msr     hcr_el2, x0
    
    mov     x0, #SPSR_VALUE
    msr     spsr_el3, x0
    
    ldr     x0, =SCR_VALUE
    msr     scr_el3, x0
    
    adr     x0, el1_entry
    msr     elr_el3, x0
    eret
    
el1_entry:
    bl      main
    wfe
    b       .


.globl get_el
get_el:
    mrs     x0, CurrentEL
    lsr     x0, x0, #2
    ret
