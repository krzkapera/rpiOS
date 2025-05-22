.section ".text.boot"  // Make sure the linker puts this at the start of the kernel image

#define SPSR_MASK_ALL 		(7 << 6)
#define SPSR_EL1h			(5 << 0)
#define SPSR_VALUE			(SPSR_MASK_ALL | SPSR_EL1h)
#define MAIN_STACK      0x400000

#define SCR_RW				(1 << 10)
#define SCR_NS				(1 << 0)
#define SCR_VALUE	    	(SCR_RW | SCR_NS)

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

    // enable floating point registers in, used in printf
    MRS     x0, cptr_el3
    BIC     x0, x0, #(1 << 31)
    BIC     x0, x0, #(1 << 10)
    MSR     cptr_el3, x0

    
    mov     x0, #3 << 20
    msr     cpacr_el1, x0	 // Enable FP/SIMD at EL1

//    switch to EL 1
   mov x0, #MAIN_STACK
   msr sp_el1, x0

   mrs x0, hcr_el2
   orr x0, x0, #(1 << 31)
   msr hcr_el2, x0

   mov x0, #(0b0101)
   msr spsr_el3, x0

   ldr     x0, =SCR_VALUE
   msr     scr_el3, x0

   adr x0, el1_entry
   msr elr_el3, x0
   eret
    
el1_entry:
    bl main
    b       .


.globl get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret
