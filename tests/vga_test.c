#include "common.h"

#include "../vga.h"

#include <stdint.h>

int hello_world_test() {
    uint8_t number_of_rows = 80;
    uint8_t number_of_columns = 25;

    // this is the VGA buffer address
    uint16_t* buffer = (uint16_t*) 0xB8000;

    struct terminal_t terminal = terminal_new(number_of_rows, number_of_columns, buffer);
    struct cursor_t cursor = {.row = 0, .column = 0};
    cursor = terminal_print(&terminal, cursor, "Hello, World!\n", 14);

    CHECK_EQ(cursor.row, 1);
    CHECK_EQ(cursor.column, 0);

    const uint8_t color = VGA_COLOR_WHITE | (VGA_COLOR_BLACK << 4);

    // check if "Hello, World!" is displayed in the buffer
    const uint8_t reference[14] = {'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', ' '};
    for (size_t idx = 0; idx < sizeof(reference); ++idx) {
        uint16_t entry = buffer[idx];
        // compare the lower byte to the reference chars
        CHECK_EQ(entry & 0xFF, reference[idx]);
        // compare the upper byte to the default fg/bg color
        CHECK_EQ((entry & 0xFF00) >> 4, color);
    }

    // check if the rest of the screen is clear
    for (size_t idx = 14; idx < number_of_rows * number_of_columns; ++idx) {
        uint16_t entry = buffer[idx];
        // compare the lower byte to the reference empty
        CHECK_EQ(entry & 0xFF, ' ');
        // compare the upper byte to the default fg/bg color
        CHECK_EQ((entry & 0xFF00) >> 4, color);
    }

    return 0;
}

int tests_main() {
    TEST(hello_world_test);

    return 0;
}
