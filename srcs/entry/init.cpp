extern "C" {
	#include <stdio/stdio.h>
	#include <utils/types.h>
	#include <input/keyboard.h>
}

#include <graphics/vga.h>

VideoGraphicsArray *vgap;

#define width 1024 // hard coded - not good please change
#define height 768 // hard coded - not good please change

uint32_t buffer[width * height]; // hard coded - not good please change
static const char* hex = "0123456789ABCDEF";

void printfHex(uint8_t key, int32_t x, int32_t y, uint32_t color) {
	char* foo = "00";

	foo[0] = hex[(key >> 4) & 0xF];
	foo[1] = hex[key & 0xF];
	vgap->PutStr(foo, x, y, color);
}

void printfHex32(uint32_t key, int32_t x, int32_t y, uint32_t color) {
	printfHex((key >> 24) & 0xFF, x, y, color);
	printfHex((key >> 16) & 0xFF, x+16, y, color);
	printfHex((key >> 8) & 0xFF, x+32, y, color);
	printfHex( key & 0xFF, x+48, y, color);
}

void printf(char* str, uint32_t key, int32_t x, int32_t y, uint32_t color) {
	vgap->PutStr(str, x, y, color);

	int l = 0;
	for (; str[l] != 0; l++);
	printfHex32(key, x + (l*8), y, color);
}

/**
 * Draw something that looks like a window
 */
void window(int32_t x, int32_t y, uint32_t w, uint32_t h) {

	vgap->FillRectangle(x, y, w, h, 0x909090); // full
	vgap->FillRectangle(x+2, y+2, w-4, 2, 0x2040E0); // top bar
	vgap->FillRectangle(x+2, y+4, w-4, 8, 0x1030A0); // top bar
	vgap->FillRectangle(x+2, y+12, w-4, 2, 0x002080); // top bar
	vgap->FillRectangle(x+2, y+14, w-4, h-28, 0xE0E0E0); // content
	vgap->FillRectangle(x+2, y+h-12, w-4, 10, 0xC0C0C0); // bottom bar
	vgap->FillRectangle(x+w-37, y+3, 10, 10, 0xD0D0D0); // button back
	vgap->FillRectangle(x+w-25, y+3, 10, 10, 0xD0D0D0); // button back
	vgap->FillRectangle(x+w-13, y+3, 10, 10, 0xD0D0D0); // button back
	vgap->PutStr("chados", x + 4, y + 5, 0xFFFFFF);

	vgap->PutChar('_', x+w-36, y+4, 0x101010);
	vgap->FillRectangle(x+w-24, y+4, 8, 8, 0x101010);
	vgap->FillRectangle(x+w-23, y+5, 6, 6, 0xD0D0D0);
	vgap->PutChar('X', x+w-12, y+4, 0x101010);

	vgap->PutStr("i totally wrote all the code for this", x + 4, y + 20, 0x000000);
}

char* memset(char* s, int c,  int len)
{
    char* p=s;
    while(len--)
    {
        *p++ = (char)c;
    }
    return s;
}

extern "C" void kernelMain(uint32_t stackPointer, const multiboot_header* multiboot_structure, uint32_t /*multiboot_magic*/) {
	/*changeBColor(0x0);
	changeFColor(0x4);

	clear_screen();

	while (true) {
		kprintln("> ");
		char* string;
		getline(string, 80);
		if(strcmp(string, "clear") == 1) {
			clear_screen();
		} else if(strcmp(string, "sus") == 1) {
			kprintln("      /----------\\");
			kprintln("      |          |");
			kprintln("      |      /------\\");
			kprintln("   /--|      |      |");
			kprintln("   |  |      \\------/");
			kprintln("   |  |          |             you are hella sus my guy.");
			kprintln("   |  |          |");
			kprintln("   \\--|    __    |");
			kprintln("      |    ||    |");
			kprintln("      |    ||    |");
			kprintln("      |____||____|");
		}
	}*/

	VideoGraphicsArray vga(multiboot_structure, buffer);

	vgap = &vga;

	int x = 20;
	int y = 0;
	int i = 0;

	while(true) {
		char string[1000];

		vgap->PutStr(">", 0, y, 0xFFFFFF);

		char character = nonblocking_getchar();

		vgap->FillRectangle(x, y + 8, 10, 2, 0xFFFFFF);

		vgap->FillRectangle(x - 10, y + 8, 10, 2, 0x0);

		if(character != 0) {
			if(character == 0x08) {
				x -= 10;

				if(x < 20) x = 20;

				vgap->FillRectangle(x, y, 10, 10, 0x0000000);
				vgap->FillRectangle(x + 10, y, 10, 10, 0x0000000);
			} else if(character == 0x0D) {
				vgap->FillRectangle(x, y, 10, 10, 0x0000000);
				x = 20;
				y += 10;
				string[1000] = {0};
					vgap->PutStr(string, 0, y + 10, 0xFFFFFF);
			} else {
				string[i] = character;
				vgap->PutStr(&character, x, y, 0xFFFFFF);
				x += 20;
				if(x > 1024) {
					x = 20;
					y += 10;
				}
				i++;
				
				if(i > 4) {
					memset(string, 0, sizeof string);

					vgap->PutStr(string, 0, y + 10, 0xFFFFFF);
				}
			}
		}

		vga.bufferToScreen();
	}
}
