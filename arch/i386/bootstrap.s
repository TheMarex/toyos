.set MB_ALIGN,   1<<0
.set MB_MEMINFO, 1<<1
.set MULTIBOOT_FLAGS, MB_ALIGN | MB_MEMINFO
.set MULTIBOOT_MAGIC, 0x1BADB002
.set MULTIBOOT_CHECKSUM, -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
# traditional 3GB offset for the kernel space which is 1GB
.set KERNEL_MEMORY_OFFSET, 0xC0000000
# we use 4KB pages on 4GB of memory -> 2^32 / 2^12 = 2^20 pages
.set NUM_PAGES, 1024*1024
.set NUM_PAGE_TABLE_ENTRIES, 1024
# every page table has 1024 entries
.set NUM_PAGE_TABLES, 1024
# everything up to the kernel page is user space
# 4KB pages -> lowe 12bit are offset
.set NUM_USER_PAGES, KERNEL_MEMORY_OFFSET >> 12
.set NUM_USER_PAGE_TABLES, NUM_USER_PAGES >> 10
# with 10bit page numbers you can have at most 1024 pages
.set NUM_KERNEL_PAGE_TABLES, NUM_PAGE_TABLES-NUM_USER_PAGE_TABLES
# directory and page table are each 4KB
.set PAGE_DIRECTORY_SIZE, NUM_PAGE_TABLE_ENTRIES*4
.set PAGE_TABLE_SIZE, NUM_PAGE_TABLE_ENTRIES*4
.set KERNEL_TABLES_SIZE, NUM_KERNEL_PAGE_TABLES*PAGE_TABLE_SIZE

.section .multiboot
.align 4
.long MULTIBOOT_MAGIC
.long MULTIBOOT_FLAGS
.long MULTIBOOT_CHECKSUM

.section .text
.global _start
.type _start, @function
_start:
        # first setup the page tables for the kernel
        # compute physical address of page table
        movl $(kernel_page_table - KERNEL_MEMORY_OFFSET), %edi
        # bit 0 and 1 are the flags for present and R/W
        movl $0x003, %esi
        movl $(NUM_KERNEL_PAGE_TABLES*NUM_PAGE_TABLE_ENTRIES), %ecx
pt_st:  movl %esi, (%edi)
        add $4, %edi
        add $0x1000, %esi
        loop pt_st

        # now make the page directory point to the page tables from above
        movl $(kernel_page_directory + 4*NUM_USER_PAGE_TABLES - KERNEL_MEMORY_OFFSET), %edi
        # load address of first page table
        # set bit 0 and bit 1 to signal present and R/W
        movl $(kernel_page_table - KERNEL_MEMORY_OFFSET + 0x003), %esi
        # set counter to number of kernel page tables
        movl $NUM_KERNEL_PAGE_TABLES, %ecx
pd_st:  movl %esi, (%edi)
        add $4, %edi
        add $0x1000, %esi
        loop pd_st

        # identity map the first page because after switching to paging the instruction
        # pointer will still contain a physical address and it would just segfault
        # this assumes that the relevant kernel code is smaller then 4k
        movl $(kernel_page_table - KERNEL_MEMORY_OFFSET + 0x003), %eax
        movl %eax, kernel_page_directory - KERNEL_MEMORY_OFFSET

        # point MMU at page directory
        movl $(kernel_page_directory - KERNEL_MEMORY_OFFSET), %ecx
        movl %ecx, %cr3

        # enable paging
        mov %cr0, %ecx
        or $0x80000000, %ecx
        mov %ecx, %cr0


        # !!! from this point on we are in paged mode !!!

        # now we make the leap into the kernel memory
        lea paged, %ecx
        jmp *%ecx

paged:
        # remove the identity mapping from the first page directory entry
        movl $0, kernel_page_directory
        mov $stack_top, %esp
        call kernel_main

        cli
loop:   hlt
        jmp loop

.size start, . - _start

.section .bss
.align 0x1000
kernel_page_directory:
        # 4 bytes per entry
        .skip PAGE_DIRECTORY_SIZE
# space for 1024 page tables with each 1024 entries of 4 bytes to map the 1GB kernel space
kernel_page_table:
        # 4 bytes per entry
        .skip KERNEL_TABLES_SIZE
        #.skip NUM_KERNEL_PAGE_TABLES*NUM_PAGE_TABLE_ENTRIES*4

# This section setups the kernel stack
stack_bottom:
.skip 16484
stack_top:

