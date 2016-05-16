#ifndef __PACKET_H__
#define __PACKET_H__

#include <stdint.h>
#include <string.h>
#include <interface.h>

struct packet_t {
	uint8_t header[2];
	uint8_t length[2];
	uint8_t command;
	uint8_t data[4096];
	uint8_t crc[4];
};

void packet_init(struct packet_t * packet, uint8_t command, uint8_t * data, size_t size);
int packet_transform(struct interface_t * iface, struct packet_t * request, struct packet_t * response, int timeout);

#endif /* __PACKET_H__ */
