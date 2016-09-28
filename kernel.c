#include "vga.h"

#ifdef ENABLE_TESTS
int tests_main();
#endif

void kernel_main()
{
#ifdef ENABLE_TESTS
    tests_main();
#else
    struct terminal_t terminal = terminal_new(80, 25, (uint16_t*) 0xB8000);
    struct cursor_t cursor = {.row = 0, .column = 0};

    cursor = terminal_print(&terminal, cursor, "Hello, World!\n", 14);
#endif
}
