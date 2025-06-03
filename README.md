czemu mount_fs z maina nie dziala

    MMU i cache dla EL1:

    mrs     x0, sctlr_el1
    bic     x0, x0, #(1 << 0)   // MMU disable
    bic     x0, x0, #(1 << 2)   // Cache disable
    bic     x0, x0, #(1 << 12)  // I-cache disable
    msr     sctlr_el1, x0
    isb


kernel_old=1 -> EL3, =0 -> EL2
