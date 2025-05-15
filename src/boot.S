.section ".text.boot"  // Make sure the linker puts this at the start of the kernel image

#define SPSR_MASK_ALL 		(7 << 6)
#define SPSR_EL1h			(5 << 0)
#define SPSR_VALUE			(SPSR_MASK_ALL | SPSR_EL1h)
#define MAIN_STACK      0x400000

.global _start  // Execution starts here

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
    mov     sp, #MAIN_STACK

    // Clean the BSS section
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f               // Quit loop if zero
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b               // Loop if non-zero

4:  nop

    // enable floating point registers, used in printf
    MRS     x0, CPTR_EL3
    BIC     x0, x0, #(1 << 31)
    BIC     x0, x0, #(1 << 10)
    MSR     CPTR_EL3, x0
    //
    

    bl main
    b       .


.globl get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret
