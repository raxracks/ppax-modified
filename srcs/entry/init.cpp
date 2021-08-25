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

int x = 25;
int y = 30;
int i = 0;

unsigned int strlen(const char *s)
{
    unsigned int count = 0;
    while(*s!='\0')
    {
        count++;
        s++;
    }
    return count;
}

int shiftChar(int character, int offset)
{
    if(character >= 97 && character <= 122) {
        character = character - offset;
    }
    return(character);
}

void DrawLineWrapper() {
	vgap->FillRectangle(5, y - 25, width - 10, 1, 0x303030);
	vgap->FillRectangle(5, y - 25, 1, 15, 0x303030);
	vgap->FillRectangle(5, y - 10, 5, 1, 0x303030);
	vgap->PutStr("~", 20, y - 10, 0x52b5b5);
}

extern "C" void kernelMain(uint32_t stackPointer, const multiboot_header* multiboot_structure, uint32_t /*multiboot_magic*/) {
	VideoGraphicsArray vga(multiboot_structure, buffer);

	vgap = &vga;

	int offset = 0;
	int spacing = 8;
	uint32_t bgColor = 0x201c24;

	vgap->Clear(bgColor);

	while(true) {
		char string[1000]; // define command array

		DrawLineWrapper();

		vgap->PutStr(">", 10, y, 0x45ac84); // draw line starter

		char character = nonblocking_getchar(); // get character

		vgap->FillRectangle(x, y + 8, 10, 2, 0xFFFFFF); // cursor

		vgap->FillRectangle(x - 10, y + 8, 10, 2, bgColor); // remove previous cursor

		if(character != 0) { // many hardcoded values below, very bad and ugly
			if(character == 0x08) {
				x -= spacing;

				if(x < 25) x = 25;

				vgap->FillRectangle(x, y, 10, 10, bgColor);
				vgap->FillRectangle(x + 10, y, 10, 10, bgColor);
			} else if(character == 0x0D) {
				vgap->FillRectangle(x, y + 8, 10, 2, bgColor); 

				i = 0;
				x = 25;
				y += 45;

				string[1000] = { 0 };
			} else {
				string[i] = character;

				if(isShiftDown() == 1) {
					offset = -32;
				} else {
					offset = 0;
				}

				char offsetChar = (character + offset);

				vgap->PutStr(&offsetChar, x, y, 0xFFFFFF);
				//vgap->PutStr(itoa(offsetChar, 16), x, y, 0xFFFFFF);

				x += spacing;

				if(x > width) {
					x = 25;
					y += 45;
				}

				i++;
			}
		}

		vga.SwapBuffers();
	}
}