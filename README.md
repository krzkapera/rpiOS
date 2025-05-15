# TODO
- UART Interrupt
- FS

przez irq przychodzi nowy kod
albo przychodzi info brak nowego kodu i idzie dalej
albo ze nowy kod, odbiera, zapisuje i restartuje

    ; ldr x0, =_start
    ; msr sp_el1, x0

    ; mrs x0, hcr_el2
    ; orr x0, x0, #(1 << 31)   // RW bit
    ; msr hcr_el2, x0

    ; mov x0, #(0b0101)
    ; msr spsr_el2, x0

    ; adr x0, el1_entry
    ; msr elr_el2, x0
    ; eret
