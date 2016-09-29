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
cursor_increment(struct terminal_t* terminal, const struct cursor_t cursor) {
    struct cursor_t new_cursor = { .row = cursor.row, .column = cursor.column + 1 };

    if (new_cursor.column >= terminal->number_of_columns) {
        new_cursor.column = 0;

        ++new_cursor.row;
        if (new_cursor.row >= terminal->number_of_rows) {
            new_cursor.row = terminal->number_of_rows-1;
            terminal_scroll_down(terminal);
        }
    }

    return new_cursor;
}

// returns to the beginning of the next line, but respects screen wrapping
// returns a new cursor_t object
struct cursor_t
cursor_newline(struct terminal_t* terminal, const struct cursor_t cursor) {
    struct cursor_t new_cursor = { .row = cursor.row + 1, .column = 0};

    if (new_cursor.row >= terminal->number_of_rows) {
        new_cursor.row = terminal->number_of_rows-1;
        terminal_scroll_down(terminal);
    }

    return new_cursor;
}

// inserts an ASCII character at the given cursor position
struct cursor_t
terminal_insert(struct terminal_t* terminal, const struct cursor_t cursor, const char c) {
    if (c == '\n') {
        return cursor_newline(terminal, cursor);
    }

    const uint16_t value = to_vga_value(terminal->foreground_color, terminal->background_color, c);
    const size_t position = terminal->number_of_columns*cursor.row + cursor.column;
    terminal->buffer[position] = value;

    return cursor_increment(terminal, cursor);
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

// prints an ASCII encoded buffer of length buffer_length
// returns the new cursor position after the string has been written
struct cursor_t
terminal_print_buffer(struct terminal_t* terminal, const struct cursor_t cursor,
                      const char* buffer, size_t buffer_length) {
    if (buffer_length == 0) {
        while(buffer[buffer_length] != '\0') buffer_length++;
    }

    struct cursor_t current_cursor = cursor;
    for (const char* buffer_end = buffer + buffer_length; buffer != buffer_end; ++buffer) {
        current_cursor = terminal_insert(terminal, current_cursor, *buffer);
    }

    return current_cursor;
}


// prints an ASCII string text that is zero terminated
// returns the new cursor position after the string has been written
struct cursor_t
terminal_print_string(struct terminal_t* terminal, const struct cursor_t cursor,
                      const char* text) {
    struct cursor_t current_cursor = cursor;

    while (*text != '\0') {
        current_cursor = terminal_insert(terminal, current_cursor, *(text++));
    }

    return current_cursor;
}

// this scrolls the terminal view down: the topmost row will get cut off.
// when we cut off rows we don't preserve them, they are gone.
void
terminal_scroll_down(struct terminal_t* terminal) {
    uint16_t* begin = terminal->buffer;
    uint16_t* new_begin = terminal->buffer + terminal->number_of_columns;
    uint16_t* end = terminal->buffer + terminal->number_of_columns * terminal->number_of_rows;
    while (new_begin != end) {
        *(begin++) = *(new_begin++);
    }

    uint16_t value = to_vga_value(terminal->foreground_color, terminal->background_color, ' ');
    while (begin != end) {
        *(begin++) = value;
    }
}

// this assumes that the size of buffer is number_of_columns * number_of_rows * uint16_t
// returns a new terminal_t object and initializes the buffer
struct terminal_t
terminal_new(const uint8_t number_of_columns, const uint8_t number_of_rows, uint16_t* buffer) {
    struct terminal_t terminal = {number_of_columns, number_of_rows, VGA_COLOR_WHITE, VGA_COLOR_BLACK, buffer};
    terminal_clear(&terminal);
    return terminal;
}
