#include <stdint.h>

enum gdt_access {
    GDT_ENTRY_ACCESS     = 1<<0,
    GDT_ENTRY_RW         = 1<<1,
    GDT_ENTRY_GROW_DOWN  = 1<<2,
    GDT_ENTRY_GROW_UP    = 0<<2,
    GDT_ENTRY_CONF_LOWER = 1<<2,
    GDT_ENTRY_CONF_SAME  = 0<<2,
    GDT_ENTRY_EXEC       = 1<<3,
    GDT_ENTRY_PRIV_0     = 0<<5,
    GDT_ENTRY_PRIV_1     = 1<<5,
    GDT_ENTRY_PRIV_2     = 2<<5,
    GDT_ENTRY_PRIV_3     = 3<<5,
    GDT_ENTRY_PRESENT    = 1<<7,
    GDT_ENTRY_VALID      = 1<<4,
};

enum gdt_flags {
    GDT_BYTE_GRANULARITY = 0,
    GDT_PAGE_GRANULARITY = 1<<2,
    GDT_16BIT_MODE       = 0,
    GDT_32BIT_MODE       = 1<<3,
};

struct global_descriptor_t {
    uint16_t limit;
    uint32_t base;
} __attribute__ ((packed));

typedef uint64_t gdt_entry_t;

gdt_entry_t gdt_encode_entry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);

void gdt_write(gdt_entry_t* gdt);

void gdt_load(const struct global_descriptor_t* gd);

