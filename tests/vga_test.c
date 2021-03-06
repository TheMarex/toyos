#include "common.h"

#include "../kernel/vga.h"

#define VGA_ADDR 0xC00B8000

#include <stdint.h>

int hello_world_test() {
    uint8_t number_of_rows = 25;
    uint8_t number_of_columns = 80;

    // this is the VGA buffer address
    uint16_t* buffer = (uint16_t*) VGA_ADDR;

    struct terminal_t terminal = terminal_new(number_of_columns, number_of_rows, buffer);
    struct cursor_t cursor = {.row = 0, .column = 0};
    cursor = terminal_print_buffer(&terminal, cursor, "Hello, World!\n", 14);

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
        CHECK_EQ((entry & 0xFF00) >> 8, color);
    }

    // check if the rest of the screen is clear
    for (size_t idx = 14; idx < number_of_rows * number_of_columns; ++idx) {
        uint16_t entry = buffer[idx];
        // compare the lower byte to the reference empty
        CHECK_EQ(entry & 0xFF, ' ');
        // compare the upper byte to the default fg/bg color
        CHECK_EQ((entry & 0xFF00) >> 8, color);
    }

    cursor = terminal_clear(&terminal);

    return 0;
}

int multiline_test() {
    uint8_t number_of_rows = 25;
    uint8_t number_of_columns = 80;

    // this is the VGA buffer address
    uint16_t* buffer = (uint16_t*) VGA_ADDR;

    struct terminal_t terminal = terminal_new(number_of_columns, number_of_rows, buffer);
    struct cursor_t cursor = {.row = 0, .column = 0};
    cursor = terminal_print_buffer(&terminal, cursor, "Foo\nBar!\nBaz!\n", 14);

    CHECK_EQ(cursor.row, 3);
    CHECK_EQ(cursor.column, 0);

    const uint8_t color = VGA_COLOR_WHITE | (VGA_COLOR_BLACK << 4);

    // check if "Foo!" is displayed in the buffer
    const uint8_t reference_1[3] = {'F', 'o', 'o'};
    for (size_t idx = 0; idx < sizeof(reference_1); ++idx) {
        uint16_t entry = buffer[idx];
        // compare the lower byte to the reference chars
        CHECK_EQ(entry & 0xFF, reference_1[idx]);
        // compare the upper byte to the default fg/bg color
        CHECK_EQ((entry & 0xFF00) >> 8, color);
    }

    const uint8_t reference_2[4] = {'B', 'a', 'r', '!'};
    for (size_t idx = 2*number_of_columns; idx < sizeof(reference_2); ++idx) {
        uint16_t entry = buffer[idx];
        // compare the lower byte to the reference chars
        CHECK_EQ(entry & 0xFF, reference_2[idx]);
        // compare the upper byte to the default fg/bg color
        CHECK_EQ((entry & 0xFF00) >> 8, color);
    }

    const uint8_t reference_3[4] = {'B', 'a', 'z', '!'};
    for (size_t idx = 3*number_of_columns; idx < sizeof(reference_3); ++idx) {
        uint16_t entry = buffer[idx];
        // compare the lower byte to the reference chars
        CHECK_EQ(entry & 0xFF, reference_3[idx]);
        // compare the upper byte to the default fg/bg color
        CHECK_EQ((entry & 0xFF00) >> 8, color);
    }

    // check if the rest of the screen is clear
    for (size_t idx = 3*number_of_columns + 4; idx < number_of_rows * number_of_columns; ++idx) {
        uint16_t entry = buffer[idx];
        // compare the lower byte to the reference empty
        CHECK_EQ(entry & 0xFF, ' ');
        // compare the upper byte to the default fg/bg color
        CHECK_EQ((entry & 0xFF00) >> 8, color);
    }

    cursor = terminal_clear(&terminal);

    return 0;
}

int scroll_test() {
    uint8_t number_of_rows = 25;
    uint8_t number_of_columns = 80;

    // this is the VGA buffer address
    uint16_t* buffer = (uint16_t*) VGA_ADDR;

    struct terminal_t terminal = terminal_new(number_of_columns, number_of_rows, buffer);
    struct cursor_t cursor = {.row = 0, .column = 0};
    cursor = terminal_print_buffer(&terminal, cursor, "Foo\nBar!\nBaz!\n", 14);
    terminal_scroll_down(&terminal);

    const uint8_t reference_2[4] = {'B', 'a', 'r', '!'};
    for (size_t idx = 0; idx < sizeof(reference_2); ++idx) {
        uint16_t entry = buffer[idx];
        // compare the lower byte to the reference chars
        CHECK_EQ(entry & 0xFF, reference_2[idx]);
    }

    const uint8_t reference_3[4] = {'B', 'a', 'z', '!'};
    for (size_t idx = number_of_columns; idx < sizeof(reference_3); ++idx) {
        uint16_t entry = buffer[idx];
        // compare the lower byte to the reference chars
        CHECK_EQ(entry & 0xFF, reference_3[idx]);
    }

    cursor = terminal_clear(&terminal);

    return 0;
}

int wrap_test() {
    uint8_t number_of_rows = 25;
    uint8_t number_of_columns = 80;

    // this is the VGA buffer address
    uint16_t* buffer = (uint16_t*) VGA_ADDR;

    struct terminal_t terminal = terminal_new(number_of_columns, number_of_rows, buffer);
    struct cursor_t cursor = {.row = 0, .column = 0};
    cursor = terminal_print_string(&terminal, cursor, "offscreen\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nonscreen");

    const uint8_t reference[8] = {'o', 'n', 's', 'c', 'r', 'e', 'e', 'n'};
    for (size_t idx = 24*number_of_columns; idx < sizeof(reference); ++idx) {
        uint16_t entry = buffer[idx];
        // compare the lower byte to the reference chars
        CHECK_EQ(entry & 0xFF, reference[idx]);
    }

    cursor = terminal_clear(&terminal);

    return 0;
}

int tests_main() {
    TEST(hello_world_test);
    TEST(multiline_test);
    TEST(scroll_test);
    TEST(wrap_test);

    return 0;
}
