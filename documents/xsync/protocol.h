#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <interface.h>

struct protocal_frame_t {
	uint8_t header1;
	uint8_t header2;
	uint8_t length[2];
	uint8_t command;
	uint8_t data[4096];
	size_t size;
	uint8_t crc;
};

int protocal_frame_transform(struct interface_t * iface, struct protocal_frame_t * request, struct protocal_frame_t * response, int timeout);

#endif /* __PROTOCOL_H__ */
