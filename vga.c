#include "vga.h"

// VGA encodes the color + character of a cell as 2byte word. the lower byte is the ASCII
// character code the upper byte encodes foreground and background color as each 4bits.
uint16_t to_vga_value(enum vga_color foreground, enum vga_color background, const char c) {
    uint16_t color = (foreground | background << 4);
    return (color << 8) | (uint16_t) c;
}

// increments the cursor but respects line-wrap
// returns a new cursor_t object
struct cursor_t
cursor_increment(const struct cursor_t cursor, uint8_t number_of_rows, uint8_t number_of_columns) {
    struct cursor_t new_cursor = { .row = cursor.row, .column = cursor.column + 1 };

    if (new_cursor.column >= number_of_columns) {
        new_cursor.column = 0;

        ++new_cursor.row;
        if (new_cursor.row >= number_of_rows) {
            new_cursor.row = 0;
        }
    }

    return new_cursor;
}

// returns to the beginning of the next line, but respects screen wrapping
// returns a new cursor_t object
struct cursor_t
cursor_newline(const struct cursor_t cursor, uint8_t number_of_rows) {
    struct cursor_t new_cursor = { .row = cursor.row + 1, .column = 0};

    if (new_cursor.row >= number_of_rows) {
        new_cursor.row = 0;
    }

    return new_cursor;
}

// inserts an ASCII character at the given cursor position
struct cursor_t
terminal_insert(struct terminal_t* terminal, const struct cursor_t cursor, const char c) {
    if (c == '\n') {
        return cursor_newline(cursor, terminal->number_of_rows);
    }

    const uint16_t value = to_vga_value(terminal->foreground_color, terminal->background_color, c);
    const size_t position = terminal->number_of_columns*cursor.row + cursor.column;
    terminal->buffer[position] = value;
    return cursor_increment(cursor, terminal->number_of_rows, terminal->number_of_columns);
}

// resets the whole buffer to whitespace with the default background color
// returns the new cursor position after the reset
struct cursor_t
terminal_clear(struct terminal_t* terminal) {
    uint16_t value = to_vga_value(terminal->foreground_color, terminal->background_color, ' ');
    uint16_t* buffer_end = terminal->buffer + terminal->number_of_rows * terminal->number_of_columns;

    for (uint16_t* buffer = terminal->buffer; buffer != buffer_end; ++buffer) {
        *buffer = value;
    }

    struct cursor_t null_cursor = {0, 0};
    return null_cursor;
}

// prints an ASCII string text of length text_length
// returns the new cursor position after the string has been written
struct cursor_t
terminal_print(struct terminal_t* terminal, const struct cursor_t cursor,
               const char* text, const size_t text_length) {
    struct cursor_t current_cursor = cursor;
    for (const char* text_end = text + text_length; text != text_end; ++text) {
        current_cursor = terminal_insert(terminal, current_cursor, *text);
    }
    return current_cursor;
}

// this assumes that the size of buffer is number_of_columns * number_of_rows * uint16_t
// returns a new terminal_t object and initializes the buffer
struct terminal_t
terminal_new(const uint8_t number_of_columns, const uint8_t number_of_rows, uint16_t* buffer) {
    struct terminal_t terminal = {number_of_columns, number_of_rows, VGA_COLOR_WHITE, VGA_COLOR_BLACK, buffer};
    terminal_clear(&terminal);
    return terminal;
}
