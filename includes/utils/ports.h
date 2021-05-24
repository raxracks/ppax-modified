#include <utils/types.h>

unsigned char port_byte_in (unsigned short port);
void port_byte_ina(unsigned short port, void* target, int lenght);
void port_byte_out (unsigned short port, unsigned char data);
unsigned short port_word_in (unsigned short port);
void port_word_ina(unsigned short port, void* target, int lenght);
void port_word_out (unsigned short port, unsigned short data);
void port_word_outa(unsigned short port, void* source, int lenght);
unsigned int port_int_in(unsigned short port);
void port_int_ina(unsigned short port, void* target, int lenght);
void port_int_out(unsigned short port, unsigned int data);
void port_byte_outb(uint16_t port, uint8_t val);
uint8_t port_byte_inb(uint16_t port);
