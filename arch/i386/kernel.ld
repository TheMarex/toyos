ENTRY(_start)

SECTIONS
{
    /* 3GB + 1MiB offset */
    . = 0xC0000000 + 1M;
    _kernel_start = .;
    /* make sure the multiboot section comes first */
    .text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000)
    {
        *(.multiboot)
        *(.text)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000)
    {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
    {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
    {
        *(COMMON)
        *(.bss)
    }
    _kernel_end = .;
}
