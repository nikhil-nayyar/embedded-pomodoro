
MEMORY{
    flash(rx): ORIGIN = 0x00000000, LENGTH = 256k
    sram(rwx): ORIGIN = 0x20000000, LENGTH = 32k
}

SECTIONS{

    PROVIDE(_stack_ptr = ORIGIN(sram) + LENGTH(sram));

    .text : 
    {
        KEEP(*(.vector_table))
        *(.text)
        *(.rodata)
        _etext = .;
    } > flash

    .data : {
        _sdata = .;
        *(.data)
        _edata = .;
    } > sram AT > flash

    .bss : {
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        _ebss = .;
        __bss_end__ = _ebss;
    } > sram

}