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

#define PORT 0x3f8          // COM1
 
static int init_serial() {
   port_byte_outb(PORT + 1, 0x00);    // Disable all interrupts
   port_byte_outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   port_byte_outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   port_byte_outb(PORT + 1, 0x00);    //                  (hi byte)
   port_byte_outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   port_byte_outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   port_byte_outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   port_byte_outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
   port_byte_outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
 
   // Check if serial is faulty (i.e: not same byte as sent)
   if(port_byte_inb(PORT + 0) != 0xAE) {
      return 1;
   }
 
   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   port_byte_outb(PORT + 4, 0x0F);
   return 0;
}

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

int is_transmit_empty() {
   return port_byte_inb(PORT + 5) & 0x20;
}
 
void write_serial(char a) {
   while (is_transmit_empty() == 0);
 
   port_byte_outb(PORT,a);
}

void serialout(char* output) {
   for(int i = 0; i < strlen(output); i++) {
      write_serial(output[i]);
   }	
}

void kernelMain(uint32_t stackPointer, const multiboot_header* multiboot_structure, uint32_t /*multiboot_magic*/) {
	init_serial();

	serialout("\e[1;1H\e[2J");
	serialout("wow serial how cool");
	
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
