.section ".text.boot"  // Make sure the linker puts this at the start of the kernel image

#define SPSR_MASK_ALL 		(7 << 6)
#define SPSR_EL1h			(5 << 0)
#define SPSR_VALUE			(SPSR_MASK_ALL | SPSR_EL1h)

.global _start  // Execution starts here

_start:
    // Check processor ID is zero (executing on main core), else hang
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // We're not on the main core, so hang in an infinite wait loop
1:  wfe
    b       1b
2:  // We're on the main core!

    // Set stack to start below our code
    ldr     x1, =__stack_top
    mov     sp, x1

    // Clean the BSS section
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f               // Quit loop if zero
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b               // Loop if non-zero

4:  nop
    bl uart_init
    bl get_el
    add x0, x0, #'0'
    bl      uart_write_byte

    // 1) EL1 jako AArch64 (HCR_EL2.RW = 1)
    mov     x0, #(1 << 31)
    msr     hcr_el2, x0


    // 2) Skopiuj konfiguracje MMU z EL2 do EL1
    mrs x1, ttbr0_el2
    msr ttbr0_el1, x1
    mrs x1, tcr_el2
    msr tcr_el1, x1
    mrs x1, sctlr_el2
    msr sctlr_el1, x1


    // 3) SPSR_EL2 → maska przerwań + EL1h
    ldr     x0, =SPSR_VALUE
    msr     spsr_el2, x0

    // 4) stos dla EL1
    ldr     x0, =__stack_top
    msr     sp_el1, x0

    // 5) adres powrotu do EL1
    adr     x0, el1_entry
    msr     elr_el2, x0

    isb
    dsb sy
    
    eret

el1_entry:


4: nop
    // b       .   // pętla w EL1
    bl      main
    b       .

.globl get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret
