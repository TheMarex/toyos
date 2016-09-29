.set MB_ALIGN,   1<<0
.set MB_MEMINFO, 1<<1
.set MULTIBOOT_FLAGS, MB_ALIGN | MB_MEMINFO
.set MULTIBOOT_MAGIC, 0x1BADB002
.set MULTIBOOT_CHECKSUM, -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

.section .multiboot
.align 4
.long MULTIBOOT_MAGIC
.long MULTIBOOT_FLAGS
.long MULTIBOOT_CHECKSUM

# This section setups the kernel stack
.section .bss
.align 16
stack_bottom:
.skip 16484
stack_top:

.section .text
.global _start
.type _start, @function
_start:
        mov $stack_top, %esp
        call kernel_main

        cli
loop:   hlt
        jmp loop

.size _start, . - _start
