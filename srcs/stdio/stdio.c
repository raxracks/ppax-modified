#include "stdio/stdio.h"
#include "utils/ports.h"
#include "utils/util.h"

/* Declaration of private functions */
int get_cursor_offset();
void set_cursor_offset(int offset);
int print_char(char c, int col, int row, char attr);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);
int currentRow = 0;

char color = WHITE_ON_BLACK;

/**********************************************************
 * Public Kernel API functions                            *
 **********************************************************/

/**
 * Print a message on the specified location
 * If col, row, are negative, we will use the current offset
 */

void changeBColor(char ncolor) {
    color = (ncolor << 4) | (color & 0xF);
}
void changeFColor(char ncolor) {
    color = (color & 0xF0) | (ncolor & 0xF);
}

void scroll_console() {

}

void kprint_at(char *message, int col, int row) {
    /* Set cursor if col/row are negative */
    int offset;
    if (col >= 0 && row >= 0)
        offset = get_offset(col, row);
    else {
        offset = get_cursor_offset();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    /* Loop through message and print it */
    int i = 0;
    while (message[i] != 0) {
        if (message[i] != ' ')
            offset = print_char(message[i++], col, row, color);
        else
            offset = print_char(message[i++], col, row, (color & 0xF0) | 0x0f);
        /* Compute row/col for next iteration */
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
}

void kprint(char *message) {
    kprint_at(message, -1, -1);
}

void scroll() {
	unsigned short *videoMemory = (unsigned short*) VIDEO_ADDRESS;
	currentRow--;

	for(int y = 1; y < 25; y++) {
		for(int i = 0; i < 80; i++) {
			print_char((videoMemory[i] & 0xFF00) | videoMemory[i + (80 * y)], i, y - 1, color);
		}
	}

	for(int i = 0; i < 80; i++) {
		print_char((videoMemory[i] & 0xFF00) | ' ', i, 24, color);
	}

	kprintln("> ");
}

void kprintln(char *message) {
	kprint_at(message, 0, currentRow);

	if(currentRow > 24) {
		scroll();
	} else {
		currentRow++;
	}
}

int getCurrentRow() {
	return currentRow;
}

void skipln() {
	currentRow++;
}

int get_chat_size(char * s) {
	char * t;
	for (t = s; *t != '\0'; t++);

	return t - s;
}

int strcmp(char* string1, char* string2) {
	for (int i = 0; i < get_chat_size(string2); i++) {
		if(string1[i] != string2[i]) return 0;
	}

	return 1;
}

/**********************************************************
 * Private kernel functions                               *
 **********************************************************/


/**
 * Innermost print function for our kernel, directly accesses the video memory 
 *
 * If 'col' and 'row' are negative, we will print at current cursor location
 * If 'attr' is zero it will use 'white on black' as default
 * Returns the offset of the next character
 * Sets the video cursor to the returned offset
 */
int print_char(char c, int col, int row, char attr) {
    unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;
    if (!attr) attr = color;

    /* Error control: print a red 'E' if the coords aren't right */
    if (col >= MAX_COLS || row >= MAX_ROWS) {
        vidmem[2*(MAX_COLS)*(MAX_ROWS)-2] = 'E';
        vidmem[2*(MAX_COLS)*(MAX_ROWS)-1] = RED_ON_WHITE;
        return get_offset(col, row);
    }

    int offset;
    if (col >= 0 && row >= 0) offset = get_offset(col, row);
    else offset = get_cursor_offset();

    if (c == '\n') {
        row = get_offset_row(offset);
        offset = get_offset(0, row+1);
    } else {
        vidmem[offset] = c;
        vidmem[offset+1] = attr;
        offset += 2;
    }

    /* Check if the offset is over screen size and scroll */
    /*if (offset >= MAX_ROWS * MAX_COLS * 2) {
        int i;
        for (i = 1; i < MAX_ROWS; i++) 
            memory_copy(get_offset(0, i) + VIDEO_ADDRESS,
                        get_offset(0, i-1) + VIDEO_ADDRESS,
                        MAX_COLS * 2);

        /* Blank last line */
        /*char *last_line = get_offset(0, MAX_ROWS-1) + VIDEO_ADDRESS;
        for (i = 0; i < MAX_COLS * 2; i++) last_line[i] = 0;

        offset -= 2 * MAX_COLS;
    }*/

    set_cursor_offset(offset);
    return offset;
}

int get_cursor_offset() {
    /* Use the VGA ports to get the current cursor position
     * 1. Ask for high byte of the cursor offset (data 14)
     * 2. Ask for low byte (data 15)
     */
    port_byte_out(REG_SCREEN_CTRL, 14);
    int offset = port_byte_in(REG_SCREEN_DATA) << 8; /* High byte: << 8 */
    port_byte_out(REG_SCREEN_CTRL, 15);
    offset += port_byte_in(REG_SCREEN_DATA);
    return offset * 2; /* Position * size of character cell */
}

void set_cursor_offset(int offset) {
    /* Similar to get_cursor_offset, but instead of reading we write data */
    offset /= 2;
    port_byte_out(REG_SCREEN_CTRL, 14);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    port_byte_out(REG_SCREEN_CTRL, 15);
    port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

void disable_cursor() {
    port_byte_out(REG_SCREEN_CTRL, 0x0A);
    port_byte_out(REG_SCREEN_DATA, 0x20);
}

void clear_screen() {
    int screen_size = MAX_COLS * MAX_ROWS;
    int i;
    char *screen = VIDEO_ADDRESS;

    for (i = 0; i < screen_size; i++) {
        screen[i*2] = ' ';
        screen[i*2+1] = (color & 0xF0) | 0x0f;
    }
    set_cursor_offset(get_offset(0, 0));
	currentRow = 0;
}

int is_ascii(int c) {
	if(c >= 0 && c<128) return 1;
	return 0;
}

int get_offset(int col, int row) { return 2 * (row * MAX_COLS + col); }
int get_offset_row(int offset) { return offset / (2 * MAX_COLS); }
int get_offset_col(int offset) { return (offset - (get_offset_row(offset)*2*MAX_COLS))/2; }
