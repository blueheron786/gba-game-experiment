/* GBA Linker Script */
ENTRY(_start)

/* Memory Layout for GBA */
MEMORY
{
    rom : ORIGIN = 0x08000000, LENGTH = 32M
    iwram : ORIGIN = 0x03000000, LENGTH = 32K
    ewram : ORIGIN = 0x02000000, LENGTH = 256K
}

__text_start = ORIGIN(rom);
__etext = ORIGIN(rom) + LENGTH(rom);
__iwram_start = ORIGIN(iwram);
__iwram_top = ORIGIN(iwram) + LENGTH(iwram);
__ewram_start = ORIGIN(ewram);
__ewram_top = ORIGIN(ewram) + LENGTH(ewram);
__sp_irq = __iwram_top - 0x60;
__sp_usr = __sp_irq - 0xa0;

SECTIONS
{
    . = ORIGIN(rom);
    
    .crt0 :
    {
        KEEP(*(.crt0))
        . = ALIGN(4);
    } >rom
    
    .text :
    {
        *(.text)
        *(.text.*)
        *(.gnu.linkonce.t.*)
        . = ALIGN(4);
    } >rom
    
    .rodata :
    {
        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r.*)
        . = ALIGN(4);
    } >rom
    
    .ARM.extab :
    {
        *(.ARM.extab*)
        . = ALIGN(4);
    } >rom
    
    .ARM.exidx :
    {
        *(.ARM.exidx*)
        . = ALIGN(4);
    } >rom
    
    . = ALIGN(4);
    __iwram_lma = .;
    
    .iwram : AT(__iwram_lma)
    {
        __iwram_start = .;
        *(.iwram)
        *(.iwram.*)
        . = ALIGN(4);
        __iwram_end = .;
    } >iwram
    
    . = __iwram_lma + SIZEOF(.iwram);
    __ewram_lma = .;
    
    .ewram : AT(__ewram_lma)
    {
        __ewram_start = .;
        *(.ewram)
        *(.ewram.*)
        . = ALIGN(4);
        __ewram_end = .;
    } >ewram
    
    . = __ewram_lma + SIZEOF(.ewram);
    __bss_lma = .;
    
    .bss : AT(__bss_lma)
    {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
    } >ewram
    
    /* Stacks */
    .stacks :
    {
        . = __iwram_top - 0x100;
        __sp_irq = .;
        . = __iwram_top - 0x60;
        __sp_usr = .;
        . = __iwram_top;
    } >iwram
    
    /* Discard unwanted sections */
    /DISCARD/ :
    {
        *(.eh_frame)
        *(.comment)
        *(.debug*)
    }
}
