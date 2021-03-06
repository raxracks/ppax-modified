#ifndef MOUSE_DEVICE_H
#define MOUSE_DEVICE_H

typedef enum {
	LEFT_CLICK   = 0x01,
	RIGHT_CLICK  = 0x02,
	MIDDLE_CLICK = 0x04
} mouse_click_t;

typedef struct {
	uint32_t magic;
	int8_t x_difference;
	int8_t y_difference;
	mouse_click_t buttons;
} mouse_device_packet_t;

#define MOUSE_MAGIC 0xFEED1234

#endif