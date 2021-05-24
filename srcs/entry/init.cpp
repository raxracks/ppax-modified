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

void window(int32_t x, int32_t y, uint32_t w, uint32_t h) { // create a window

	vgap->FillRectangle(x, y, w, h, 0x909090); // border
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

	vgap->PutStr("hello", x + 4, y + 20, 0x000000);

	vgap->PutStr("circle", x + 4, y + 30, 0x000000);
}

char* itoa(int value, int base = 10) // matei
{
    char* str = "";

    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

int x = 20;
	int y = 0;
	int i = 0;

	int windowX = 0;
	int windowY = 0;

	void HandleMessage(char* message) {
		if(strcmp(message, "createwindow") == 1) {
			window(windowX, windowY, 500, 500);
		} else if(strcmp(message, "right") == 1) {
			windowX += 10;
		} else if(strcmp(message, "left") == 1) {
			windowX -= 10;
		} else if(strcmp(message, "up") == 1) {
			windowY -= 10;
		} else if(strcmp(message, "down") == 1) {
			windowY += 10;
		} else if(strcmp(message, "clear") == 1) {
			vgap->Clear();

			x = 20;
			y = 0;
		}
	}

extern "C" void kernelMain(uint32_t stackPointer, const multiboot_header* multiboot_structure, uint32_t /*multiboot_magic*/) {
	VideoGraphicsArray vga(multiboot_structure, buffer);

	vgap = &vga;

	

	while(true) {
		//vgap->Clear();

		char string[1000]; // define command array

		vgap->PutStr(">", 0, y, 0xFFFFFF); // draw line starter

		char character = nonblocking_getchar(); // get character

		vgap->FillRectangle(x, y + 8, 10, 2, 0xFFFFFF); // cursor

		vgap->FillRectangle(x - 10, y + 8, 10, 2, 0x0); // remove previous cursor

		if(character != 0) { // many hardcoded values below, very bad and ugly
			if(character == 0x08) {
				x -= 10;

				if(x < 20) x = 20;

				vgap->FillRectangle(x, y, 10, 10, 0x0000000);
				vgap->FillRectangle(x + 10, y, 10, 10, 0x0000000);
			} else if(character == 0x0D) {
				vgap->FillRectangle(x, y + 8, 10, 2, 0x0); 

				i = 0;
				x = 20;
				y += 10;

				HandleMessage(string);

				string[1000] = { 0 };
			} else {
				string[i] = character;
				vgap->PutStr(&character, x, y, 0xFFFFFF);
				x += 10;
				if(x > width) {
					x = 20;
					y += 10;
				}

				i++;
			}

			
		}

		//circleSimple(x, 300, 50, 0xE0E0E0);

		vga.SwapBuffers();
	}
}