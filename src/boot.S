#include "sys/sysregs.h"
#include "mmu/mmu.h"
#include "mmu/mm.h"

#define OSC_FREQ        54000000

/* changes tmp1, tmp2 only */
.macro create_table_entry, tbl, ntbl, va, shift, flags, tmp1, tmp2
    lsr \tmp1, \va, #\shift
    and \tmp1, \tmp1, #ENTRIES_PER_TABLE - 1
    mov \tmp2, \ntbl
    orr \tmp2, \tmp2, #\flags
    str \tmp2, [\tbl, \tmp1, lsl #3]
.endm

/* changes vstart, vend, pa, tmp1 */
.macro create_block_map, pmd, vstart, vend, pa, flags, tmp1
    lsr \vstart, \vstart, #SECTION_SHIFT
    and \vstart, \vstart, #ENTRIES_PER_TABLE - 1
    lsr \vend, \vend, #SECTION_SHIFT
    sub \vend, \vend, #1
    and \vend, \vend, #ENTRIES_PER_TABLE - 1
    lsr \pa, \pa, #SECTION_SHIFT
    lsl \pa, \pa, #SECTION_SHIFT
    ldr \tmp1, =\flags
    orr \pa, \pa, \tmp1
2:
    str \pa, [\pmd, \vstart, lsl #3]
    add \pa, \pa, #SECTION_SIZE
    add \vstart, \vstart, #1
    cmp \vstart, \vend
    b.le 2b
.endm

.section ".text.boot"
.globl _start
_start:
    /* Set up CNTFRQ_EL0 */
    ldr x0, =OSC_FREQ
    msr CNTFRQ_EL0, x0

    /* Set up CNTVOFF_EL2 */
    /* physical timer value = virtual timer value */
    msr CNTVOFF_EL2, xzr

    /* Enable FP/SIMD */
    /* bit 10 (TFP) is set to 0, others are res0 */
    msr CPTR_EL3, xzr

    /* Set up SCR */
    mov x0, #SCR_VAL
    msr SCR_EL3, x0

    /* Set up ACTLR */
    mov x0, #ACTLR_VAL
    msr ACTLR_EL3, x0

    /* Set SMPEN */
    mov x0, #CPUECTLR_EL1_SMPEN
    msr CPUECTLR_EL1, x0

    bl      setup_more_regs

    mrs x6, MPIDR_EL1
    and x6, x6, #0x3
    cbz x6, master
    b	proc_hang

proc_hang:
    wfe
	b proc_hang

setup_more_regs:
    /*
     * Set up SCTLR_EL2
     * All set bits below are res1. LE, no WXN/I/SA/C/A/M
     */
    ldr x0, =0x30c50830
    msr SCTLR_EL2, x0

    /* setup SCTLR_EL1 */
    ldr x0, =SCTLR_EL1_VAL_MMU_DISABLED
    msr SCTLR_EL1, x0

    /* setup HCR_EL2 */
    ldr x0, =HCR_EL2_VAL
    msr HCR_EL2, x0

    /* setup SPSR_EL3 */
    ldr x0, =SPSR_EL3_VAL
    msr SPSR_EL3, x0

    /* setup CPACR_EL1 */
    ldr x0, =CPACR_EL1_VAL
    msr CPACR_EL1, x0

    /* setup TCR_EL1 */
    ldr x0, =TCR_EL1_VAL
    msr TCR_EL1, x0

    /* setup MAIR_EL1 */
    ldr x0, =MAIR_EL1_VAL
    msr MAIR_EL1, x0

    ret

master:
    bl drop_to_el1
    mov sp, #LOW_MEMORY
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl memzero
    // bl map_identity
    // bl map_high
    adr x0, _start
    adr x1, KERNEL_PA_BASE
    str x0, [x1]
    // adrp x0, id_pg_dir
    // msr ttbr0_el1, x0
    // adrp x0, high_pg_dir
    // msr ttbr1_el1, x0
    // mrs x0, sctlr_el1
    // mov x1, #SCTLR_EL1_MMU_ENABLED
    // orr x0, x0, x1
    // msr sctlr_el1, x0
    // ldr x2, =LINEAR_MAP_BASE
    // add sp, sp, x2
    adr x1, main
    // add x1, x1, x2
    // mov x0, #0
    blr x1
    b proc_hang

drop_to_el1:
    adr x0, el1_entry
    msr ELR_EL3, x0
    eret
el1_entry:
    ret

// map_identity:
//     mov x29, x30
//     adrp x0, id_pg_dir
//     mov x1, #ID_MAP_TABLE_SIZE
//     bl memzero
//     adrp x0, id_pg_dir
//     add x1, x0, #PAGE_SIZE
//     eor x4, x4, x4
//     create_table_entry x0, x1, x4, PGD_SHIFT, TD_KERNEL_TABLE_FLAGS, x2, x3
//     add x0, x0, #PAGE_SIZE
//     add x1, x1, #PAGE_SIZE
//     create_table_entry x0, x1, x4, PUD_SHIFT, TD_KERNEL_TABLE_FLAGS, x2, x3
//     mov x0, x1
//     eor x2, x2, x2
//     ldr x3, =ID_MAP_SIZE
//     eor x4, x4, x4
//     create_block_map x0, x2, x3, x4, TD_KERNEL_BLOCK_FLAGS, x5
//     mov x30, x29
//     ret

// map_high:
//     mov x29, x30
//     adrp x0, high_pg_dir
//     mov x1, #HIGH_MAP_TABLE_SIZE
//     bl memzero
//     adrp x0, high_pg_dir
//     add x1, x0, #PAGE_SIZE
//     ldr x4, =LINEAR_MAP_BASE
//     create_table_entry x0, x1, x4, PGD_SHIFT, TD_KERNEL_TABLE_FLAGS, x2, x3
//     add x0, x0, #PAGE_SIZE
//     add x1, x1, #PAGE_SIZE
//     ldr x4, =LINEAR_MAP_BASE
//     ldr x5, =PUD_ENTRY_MAP_SIZE
//     create_table_entry x0, x1, x4, PUD_SHIFT, TD_KERNEL_TABLE_FLAGS, x2, x3
//     add x1, x1, #PAGE_SIZE
//     add x4, x4, x5
//     create_table_entry x0, x1, x4, PUD_SHIFT, TD_KERNEL_TABLE_FLAGS, x2, x3
//     add x1, x1, #PAGE_SIZE
//     add x4, x4, x5
//     create_table_entry x0, x1, x4, PUD_SHIFT, TD_KERNEL_TABLE_FLAGS, x2, x3
//     add x1, x1, #PAGE_SIZE
//     add x4, x4, x5
//     create_table_entry x0, x1, x4, PUD_SHIFT, TD_KERNEL_TABLE_FLAGS, x2, x3
//     ldr x10, =HIGH_MAP_FIRST_START
//     ldr x11, =HIGH_MAP_FIRST_END
//     ldr x12, =HIGH_MAP_SECOND_START
//     ldr x13, =HIGH_MAP_SECOND_END
//     ldr x14, =HIGH_MAP_THIRD_START
//     ldr x15, =HIGH_MAP_THIRD_END
//     ldr x16, =HIGH_MAP_FOURTH_START
//     ldr x17, =HIGH_MAP_FOURTH_END
//     ldr x18, =HIGH_MAP_DEVICE_START
//     ldr x19, =HIGH_MAP_DEVICE_END
//     ldr x20, =FIRST_START
//     ldr x21, =SECOND_START
//     ldr x22, =THIRD_START
//     ldr x23, =FOURTH_START
//     ldr x24, =DEVICE_START
//     add x0, x0, #PAGE_SIZE
//     mov x2, x10
//     mov x3, x11
//     mov x4, x20
//     create_block_map x0, x2, x3, x4, TD_KERNEL_BLOCK_FLAGS, x5
//     add x0, x0, #PAGE_SIZE
//     mov x2, x12
//     mov x3, x13
//     mov x4, x21
//     create_block_map x0, x2, x3, x4, TD_KERNEL_BLOCK_FLAGS, x5
//     add x0, x0, #PAGE_SIZE
//     mov x2, x14
//     mov x3, x15
//     mov x4, x22
//     create_block_map x0, x2, x3, x4, TD_KERNEL_BLOCK_FLAGS, x5
//     add x0, x0, #PAGE_SIZE
//     mov x2, x16
//     mov x3, x17
//     mov x4, x23
//     create_block_map x0, x2, x3, x4, TD_KERNEL_BLOCK_FLAGS, x5
//     mov x2, x18
//     mov x3, x19
//     mov x4, x24
//     create_block_map x0, x2, x3, x4, TD_DEVICE_BLOCK_FLAGS, x5
//     mov x30, x29
//     ret

.globl memzero
memzero:
	str xzr, [x0], #8
	subs x1, x1, #8
	b.gt memzero
	ret

.globl get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret
