@
@ GBA startup code (crt0.s)
@

    .section .crt0,"ax"
    .global _start
    .global _rom_header
    .arm
    
    @ Nintendo logo and header
_rom_header:
    b _start                @ Branch to start
    .fill 156,1,0           @ Nintendo logo (filled with zeros for now)
    .ascii "HELLO"          @ Game title (12 bytes)
    .fill 7,1,0
    .ascii "01"             @ Game code (4 bytes)  
    .fill 2,1,0
    .ascii "00"             @ Maker code (2 bytes)
    .byte 0x96              @ Fixed value
    .byte 0x00              @ Main unit code
    .byte 0x00              @ Device type
    .fill 7,1,0             @ Reserved
    .byte 0x00              @ Software version
    .byte 0x00              @ Header checksum (will be fixed by gbafix)
    .fill 2,1,0             @ Reserved

_start:
    @ Set interrupt disable flag
    mov r0, #0x12           @ IRQ mode
    msr cpsr_c, r0
    ldr sp, =__sp_irq       @ Set IRQ stack
    
    mov r0, #0x1f           @ System mode
    msr cpsr_c, r0
    ldr sp, =__sp_usr       @ Set user stack
    
    @ Jump to main
    ldr r0, =main
    bx r0

    @ In case main returns
_loop:
    b _loop

    .pool
