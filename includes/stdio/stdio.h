#include <utils/types.h>

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4

/* Screen i/o ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

/* Public kernel API */
void clear_screen();
void kprint_at(char *message, int col, int row);
void kprint(char *message);
void kprintln(char *message);
void skipln();
void changeBColor(char ncolor);
void changeFColor(char ncolor);
int get_cursor_offset();
void set_cursor_offset(int offset);
int is_ascii(int c);
int strcmp(char* string1, char* string2);
int get_char_size(char* s);
int getCurrentRow();
