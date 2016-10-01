#include "gdt.h"

// encode entry as 8 byte blob
//  byte |   3    |   2    |   1    |   0    |
//  word |--------|--------|--------|--------|
//   0   |      base       |      limit      |
//   1   | base  | fl | li | access |  base  |
gdt_entry_t gdt_encode_entry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    gdt_entry_t result = 0;
    result |= limit & 0x0000FFFF;                             // byte 0, 1
    result |= ((uint64_t)(base & 0x00FFFFFF)) << 16;          // byte 2, 3, 4
    result |=  (uint64_t) access << 40;                       // byte 5
    result |= ((uint64_t)((limit & 0x000F0000) >> 16)) << 48; // lower byte 6
    result |= ((uint64_t)(flags & 0x0F)) << 52;               // higher byte 6
    result |= ((uint64_t)((base & 0xFF000000)) >> 24) << 56;  // byte 7, 8
    return result;
}

// this function assumes there is enough space allocated for the gdt table at gdt (8 bytes * 5)
void gdt_write(gdt_entry_t* gdt) {
    const uint8_t default_flags = GDT_PAGE_GRANULARITY | GDT_32BIT_MODE;
    const uint8_t default_access = GDT_ENTRY_RW | GDT_ENTRY_GROW_UP | GDT_ENTRY_VALID | GDT_ENTRY_PRESENT;
    // null entry
    *(gdt++) = gdt_encode_entry(0, 0, 0, 0);
    // kernel code segment
    *(gdt++) = gdt_encode_entry(0x00000000, 0xFFFFFFFF, default_access | GDT_ENTRY_PRIV_0 | GDT_ENTRY_EXEC, default_flags);
    // kernel data segment
    *(gdt++) = gdt_encode_entry(0x00000000, 0xFFFFFFFF, default_access | GDT_ENTRY_PRIV_0, default_flags);
    // user code segment
    *(gdt++) = gdt_encode_entry(0x00000000, 0xFFFFFFFF, default_access | GDT_ENTRY_PRIV_3 | GDT_ENTRY_EXEC, default_flags);
    // user data segment
    *(gdt++) = gdt_encode_entry(0x00000000, 0xFFFFFFFF, default_access | GDT_ENTRY_PRIV_3, default_flags);
}

void gdt_load(const struct global_descriptor_t* gd) {
    __asm__ __volatile__ (
        "lgdt (%[gd_ptr])\n\t"
        "jmp $0x8, $reload_cs\n\t"
        "reload_cs:\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        : :
        [gd_ptr] "r" (gd)
        : "ax");
}
