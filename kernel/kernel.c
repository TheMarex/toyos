#include "vga.h"
#include "gdt.h"

#ifdef ENABLE_TESTS
int tests_main();
void kernel_main()
{
    test_main();
}
#else

void kernel_main()
{
    gdt_entry_t table[5];
    gdt_write(table);

    const struct global_descriptor_t gd = {.base = (uint32_t)table, .limit = sizeof(table)-1};
    gdt_load(&gd);

    struct terminal_t terminal = terminal_new(80, 25, (uint16_t*) 0xC00B8000);
    struct cursor_t cursor = {.row = 0, .column = 0};

    cursor = terminal_print_string(&terminal, cursor, "Hello, World!\n");
    cursor = terminal_print_string(&terminal, cursor, "Foo!\nBar\n");
#endif
}
