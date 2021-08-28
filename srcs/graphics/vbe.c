#include <graphics/vbe.h>

/*

As the is no memory management the Offscreen buffer is allocated elsewhere and passed in.

This class could be re-worked as a Canvas with out to many changes

*/            
InitVBE(const multiboot_header * boot_header, uint32_t * _buffer) {
    width = boot_header->framebuffer_width;
    height = boot_header->framebuffer_height;
    VBEscreen = (uint32_t*)boot_header->framebuffer_addr;
    
    VBEbuffer = _buffer;

    // initialiase to 0    
    for (uint32_t i = 0; i < (width * (height)); i++) {
        VBEbuffer[i] = (uint32_t)0;
    }
}


void VBEPutPixel(int32_t x, int32_t y, uint32_t color) {
    if (x < 0 || width <= x || y < 0 || height <= y)
        return;
        
    VBEbuffer[ width * y + x] = color;
}

void VBEFillRectangle(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t color) {
    uint32_t i = width* (y - 1);
    
    // test if the Rectangle will be clipped (will it be fully in the screen or partially)
    if (x >= 0 && x+w < width && y >= 0 && y+h < height) {
        // fully drawn
        i += x + w;
        for(int32_t yy = h; yy > 0; yy--) {
            i += width - w;
            for(int32_t xx = w; xx > 0; xx--) {
               VBEbuffer[i++] = color;
            }
        }
    } else {
        // clipped
        for(int32_t yy = y; yy < y+h; yy++) {
            i += width;
            for(int32_t xx = x; xx < x+w; xx++) {
                if (xx >= 0 && xx < width && yy >= 0 && yy < height) 
                    VBEbuffer[i + xx] = color;
            }
        }
    }
}

/**
 * Copy the screen buffer to the screen
 */
void VBESwapBuffers() {
    
    // do the multiply once and test against 0
    for(int i = width * height; i >= 0; i--) {
        VBEscreen[i] = VBEbuffer[i];
    }
}

void VBEClear(uint32_t color) {
    for(int i = width * height; i >= 0; i--) {
        VBEbuffer[i] = color;
    }
}

void VBEPutChar(char ch, int32_t x, int32_t y, uint32_t color) {
    uint32_t px = 0;  // position in the charactor - an 8x8 font = 64 bits
    uint64_t bCh = FONT[ch];
    
    // check if it will be drawn off screen
    if (x+8 < 0 || x > width || y+8 < 0 || y > height)
        return;
    
    // test if the charactor will be clipped (will it be fully in the screen or partially)
    if (x >= 0 && x+8 < width && y >= 0 && y+8 < height) {
        // fully in the screen - pre calculate some of the values
        // so there is less going on in the loop
        int i = width * (y - 1) + x + 8;
        int incAmount = width - 8;
        for (int yy = 7; yy >= 0; yy-- ) {
            i += incAmount;
            for (int xx = 7; xx >= 0; xx-- ) {
                // test if a pixel is drawn here
                if ((bCh >> px++) & 1) {
                    VBEbuffer[i] = color;
                }
                i++;
            }
        } 
    } else {
        // partially in the screen
        int xpos = 0;
        int i = width* (y - 1);    
        for (int yy = 0; yy < 8; yy++ ) {
            i += width;
            xpos = x;
            for (int xx = 7; xx >= 0; xx-- ) {
                // test if a pixel is drawn here
                if ((bCh >> px++) & 1) {
                    // test if the pixel will be off screen
                    if (xpos > 0 && xpos < width && yy+y > 0 && yy+y < height)
                        VBEbuffer[ i + xpos] = color;
                }
                xpos++;
            }
        } 
    }
}


void VBEPutStr(char* ch, int32_t x, int32_t y, uint32_t color) {
    for (int32_t i = 0; ch[i] != 0; i++, x+=8) {
        VBEPutChar(ch[i], x, y, color);
    }
}

