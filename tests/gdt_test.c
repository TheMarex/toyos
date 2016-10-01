#include "common.h"
#include "../kernel/gdt.h"

int default_table_test() {
    gdt_entry_t entry = gdt_encode_entry(0xDEADBEAF, 0x00054321, 0xBA, 0xF9);
    __asm__("hlt\n\t");
    // lower limit
    CHECK_EQ(entry & 0x000000000000FFFF, 0x0000000000004321);
    // lower base
    CHECK_EQ(entry & 0x000000FFFFFF0000, 0x000000ADBEAF0000);
    // access
    CHECK_EQ(entry & 0x0000FF0000000000, 0x0000BA0000000000);
    // upper limit
    CHECK_EQ(entry & 0x000F000000000000, 0x0005000000000000);
    // flags
    CHECK_EQ(entry & 0x00F0000000000000, 0x0090000000000000);
    // upper base
    CHECK_EQ(entry & 0xFF00000000000000, 0xDE00000000000000);
    return 0;
}

int tests_main() {
    TEST(default_table_test);
    return 0;
}
