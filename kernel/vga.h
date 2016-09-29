#include <stdint.h>
#include <stddef.h>

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

struct cursor_t {
    uint8_t row;
    uint8_t column;
};

struct terminal_t {
    const uint8_t number_of_columns;
    const uint8_t number_of_rows;
    enum vga_color foreground_color;
    enum vga_color background_color;
    uint16_t* const buffer;
};

struct cursor_t
cursor_increment(struct terminal_t* terminal, const struct cursor_t cursor);

struct cursor_t
cursor_newline(struct terminal_t* terminal, const struct cursor_t cursor);

struct cursor_t
terminal_insert(struct terminal_t* terminal, const struct cursor_t cursor, const char c);

struct cursor_t
terminal_clear(struct terminal_t* terminal);

struct cursor_t
terminal_print_buffer(struct terminal_t* terminal, const struct cursor_t cursor,
                      const char* buffer, const size_t buffer_length);

struct cursor_t
terminal_print_string(struct terminal_t* terminal, const struct cursor_t cursor,
                      const char* string);

void
terminal_scroll_down(struct terminal_t* terminal);

struct terminal_t
terminal_new(const uint8_t number_of_columns, const uint8_t number_of_rows, uint16_t* buffer);

