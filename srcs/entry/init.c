#include <stdio/stdio.h>
#include <utils/types.h>
#include <input/keyboard.h>
#include <graphics/vbe.h>
#include <interpreter/absorba.h>

#define width 1024 // hard coded - not good please change
#define height 768 // hard coded - not good please change

uint32_t buffer[width * height]; // hard coded - not good please change

int x = 25;
int y = 30;
int i = 0;

int shiftChar(int character, int offset)
{
    if(character >= 97 && character <= 122) {
        character = character - offset;
    }
    return(character);
}

void DrawLineWrapper() {
	VBEFillRectangle(5, y - 25, width - 10, 1, 0x303030);
	VBEFillRectangle(5, y - 25, 1, 15, 0x303030);
	VBEFillRectangle(5, y - 10, 5, 1, 0x303030);
	VBEPutStr("~", 20, y - 10, 0x52b5b5);
}

int spacing = 8;

void K_Print(char* output) {
	VBEPutStr(output, x, y, 0xffffff);
	x += spacing;
}

int K_GetCursorX() {
	return x;
}

int K_GetCursorY() {
	return y;
}

void K_SetCursor(int tx, int ty) {
	x = tx;
	y = ty;
}

void K_NewLine() {
	y += 45;
	x = 25;
}

void kernelMain(uint32_t stackPointer, const multiboot_header* multiboot_structure, uint32_t /*multiboot_magic*/) {
	InitVBE(multiboot_structure, buffer);

	int offset = 0;
	
	uint32_t bgColor = 0x201c24;

	VBEClear(bgColor);

	while(true) {
		char string[1000]; // define command array

		DrawLineWrapper();

		VBEPutStr(">", 10, y, 0x45ac84); // draw line starter

		char character = nonblocking_getchar(); // get character

		VBEFillRectangle(x, y + 8, 10, 2, 0xFFFFFF); // cursor

		VBEFillRectangle(x - 10, y + 8, 10, 2, bgColor); // remove previous cursor

		if(character != 0) { // many hardcoded values below, very bad and ugly
			if(character == 0x08) {
				x -= spacing;

				if(x < 25) x = 25;

				VBEFillRectangle(x, y, 10, 10, bgColor);
				VBEFillRectangle(x + 10, y, 10, 10, bgColor);
			} else if(character == 0x0D) {
				VBEFillRectangle(x, y + 8, 10, 2, bgColor); 

				i = 0;
				x = 25;
				y += 45;

				if(strcmp(string, "test") == 0) {
					RunAsborbaProgram();
				}
			} else {
				string[i] = character;

				if(isShiftDown() == 1) {
					offset = -32;
				} else {
					offset = 0;
				}

				char offsetChar = (character + offset);

				VBEPutChar(offsetChar, x, y, 0xfffffff);

				x += spacing;

				if(x > width) {
					x = 25;
					y += 45;
				}

				i++;
			}
		}

		VBESwapBuffers();
	}
}