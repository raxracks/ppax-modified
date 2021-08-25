#include "input/keyboard.h"
#include <stdio/stdio.h>
#include <utils/types.h>
#include <utils/codes.h>

/**********************************************************
 * Private kernel functions                               *
 **********************************************************/

int shiftDown = 0;

uint8_t kybrd_ctrl_read_status(){
	return port_byte_inb(KYBRD_CTRL_STATS_REG);
}
//send command byte to controller
void kybrd_ctrl_send_cmd (uint8_t cmd) {
	//wait for controller input buffer to be clear
	while (1)
		if ( (kybrd_ctrl_read_status () & KYBRD_CTRL_STATS_MASK_IN_BUF) == 0)
			break;
		port_byte_outb(KYBRD_CTRL_CMD_REG, cmd);
}

//read encoder buffer
uint8_t kybrd_enc_read_buf () {
	return port_byte_inb(KYBRD_ENC_INPUT_BUF);
}

//write command byte to encoder
void kybrd_enc_send_cmd (uint8_t cmd) {
	//wait for controller input buffer to be clear
	while (1)
		if ( (kybrd_ctrl_read_status () & KYBRD_CTRL_STATS_MASK_IN_BUF) == 0)
			break;
		// send command byte to encoder
		port_byte_outb(KYBRD_ENC_CMD_REG, cmd);
}

/**********************************************************
 * Public Kernel API functions                            *
 **********************************************************/

int isShiftDown() {
	return shiftDown;
}

void getline(char* string, int len){
	uint8_t i=0;
	int flag = 0;
	char temp = 0;
	int column = 3;

	memset(string,0,len);
	while(i<len && temp != 0x0D){
		temp = getchar();
		if((is_ascii(temp) == 1) && temp != 0x0D){ //0x0D is ENTER
			if(temp == 0x08)
			{   //backspace
				column -= 2;
				if(column < 2) column = 2;
				kprint_at(" ", column, getCurrentRow() - 1); //cancel last char
				set_cursor_offset(get_cursor_offset() - 2);
				i--;
			}
			else
			{
				kprint(&temp);
				string[i] = temp;
				i++;
			}

			column++;
		}
	}
	string[i] = 0x0A; //endline
}

char getchar() {
	uint8_t code = 0;
	uint8_t key = 0;
	while(1) {
		if (kybrd_ctrl_read_status () & KYBRD_CTRL_STATS_MASK_OUT_BUF) {
			code = kybrd_enc_read_buf ();
			if(code <= 0x58){
				key = _kkybrd_scancode_std [code];
				break;
			}
		}
	}
	return key;
}

char nonblocking_getchar() {
	uint8_t code = 0;
	uint8_t key = 0;
		if (kybrd_ctrl_read_status () & KYBRD_CTRL_STATS_MASK_OUT_BUF) {
			code = kybrd_enc_read_buf ();

			if(code == 0x2A) {
				shiftDown = 1; // shift down
				return 0;
			}

			if(code == 0xAA) {
				shiftDown = 0; // shift up
				return 0;
			}

			if(code <= 0x58){
				key = _kkybrd_scancode_std [code];
			}

			//key = code;
		}
	return key;
}
