#include <crc32.h>
#include <packet.h>

enum packet_state_t {
	PACKET_STATE_HEADER0,
	PACKET_STATE_HEADER1,
	PACKET_STATE_LENGTH0,
	PACKET_STATE_LENGTH1,
	PACKET_STATE_COMMAND,
	PACKET_STATE_DATA,
	PACKET_STATE_CRC0,
	PACKET_STATE_CRC1,
	PACKET_STATE_CRC2,
	PACKET_STATE_CRC3,
};

struct packet_get_ctx_t {
	struct packet_t * packet;
	enum packet_state_t state;
	int index;
};

static inline uint64_t time_now(void)
{
	struct timeval time;
	gettimeofday(&time, 0);
	return (uint64_t)(time.tv_sec * 1000 + time.tv_usec / 1000);
}

static inline uint16_t packet_length(struct packet_t * packet)
{
	return ((packet->length[0] << 8) | (packet->length[1] << 0));
}

static inline uint16_t packet_dsize(struct packet_t * packet)
{
	return packet_length(packet) - 5;
}

static inline uint32_t packet_crc(struct packet_t * packet)
{
	uint32_t crc = 0;

	if(packet)
	{
		crc = crc32(crc, &(packet->header[0]), 2);
		crc = crc32(crc, &(packet->length[0]), 2);
		crc = crc32(crc, &(packet->command), 1);
		crc = crc32(crc, &(packet->data[0]), packet_dsize(packet));
	}
	return crc;
}

static void packet_put(struct interface_t * iface, struct packet_t * packet)
{
	uint16_t dsize = packet_dsize(packet);

	interface_write(iface, &(packet->header), 2);
	interface_write(iface, &(packet->length), 2);
	interface_write(iface, &(packet->command), 1);
	if(dsize)
		interface_write(iface, &(packet->data[0]), dsize);
	interface_write(iface, &(packet->crc), 4);
}

static int packet_get_byte(struct packet_get_ctx_t * ctx, uint8_t c)
{
	uint8_t * p = (uint8_t *)(ctx->packet);
	uint32_t crc;
	uint16_t length;

	p[ctx->index++] = c;

	switch(ctx->state)
	{
	case PACKET_STATE_HEADER0:
		if(c == 'X')
		{
			ctx->state = PACKET_STATE_HEADER1;
		}
		else
		{
			ctx->index = 0;
			ctx->state = PACKET_STATE_HEADER0;
		}
		break;

	case PACKET_STATE_HEADER1:
		if(c == 'x')
		{
			ctx->state = PACKET_STATE_LENGTH0;
		}
		else
		{
			ctx->index = 0;
			ctx->state = PACKET_STATE_HEADER0;
		}
		break;

	case PACKET_STATE_LENGTH0:
		ctx->state = PACKET_STATE_LENGTH1;
		break;

	case PACKET_STATE_LENGTH1:
		ctx->state = PACKET_STATE_COMMAND;
		break;

	case PACKET_STATE_COMMAND:
		ctx->state = PACKET_STATE_DATA;
		length = (p[2] << 8) | (p[3] << 0);
		if(ctx->index >= length)
			ctx->state = PACKET_STATE_CRC0;
		break;

	case PACKET_STATE_DATA:
		length = (p[2] << 8) | (p[3] << 0);
		if(ctx->index >= length)
			ctx->state = PACKET_STATE_CRC0;
		break;

	case PACKET_STATE_CRC0:
		ctx->packet->crc[0] = c;
		ctx->state = PACKET_STATE_CRC1;
		break;

	case PACKET_STATE_CRC1:
		ctx->packet->crc[1] = c;
		ctx->state = PACKET_STATE_CRC2;
		break;

	case PACKET_STATE_CRC2:
		ctx->packet->crc[2] = c;
		ctx->state = PACKET_STATE_CRC3;
		break;

	case PACKET_STATE_CRC3:
		ctx->packet->crc[3] = c;
		ctx->index = 0;
		ctx->state = PACKET_STATE_HEADER0;
		crc  = (ctx->packet->crc[0] << 24) & 0xff000000;
		crc |= (ctx->packet->crc[1] << 16) & 0x00ff0000;
		crc |= (ctx->packet->crc[2] <<  8) & 0x0000ff00;
		crc |= (ctx->packet->crc[3] <<  0) & 0x000000ff;
		if(packet_crc(ctx->packet) == crc)
			return 0;
		break;

	default:
		ctx->index = 0;
		ctx->state = PACKET_STATE_HEADER0;
		break;
	}

	return -1;
}

static int packet_get(struct interface_t * iface, struct packet_t * packet, int timeout)
{
	struct packet_get_ctx_t ctx;
	uint64_t time, end;
	uint8_t c;
	int ret = -1;

	ctx.packet = packet;
	ctx.state = PACKET_STATE_HEADER0;
	ctx.index = 0;

	end = time_now() + timeout;
	do {
		if(interface_read(iface, &c, 1) == 1)
			ret = packet_get_byte(&ctx, c);
		time = time_now();
	} while((ret < 0) && (time <= end));

	return ret;
}

void packet_init(struct packet_t * packet, uint8_t command, uint8_t * data, size_t size)
{
	uint32_t crc;

	if(!data)
		size = 0;
	packet->header[0] = 'X';
	packet->header[1] = 'x';
	packet->length[0] = ((size + 5) >> 8) & 0xff;
	packet->length[1] = ((size + 5) >> 0) & 0xff;
	packet->command = command;
	if(size > 0)
		memcpy(&(packet->data[0]), data, size);
	crc = packet_crc(packet);
	packet->crc[0] = (crc >> 24) & 0xff;
	packet->crc[1] = (crc >> 16) & 0xff;
	packet->crc[2] = (crc >>  8) & 0xff;
	packet->crc[3] = (crc >>  0) & 0xff;
}

int packet_transfer(struct interface_t * iface, struct packet_t * request, struct packet_t * response, int timeout)
{
	packet_put(iface, request);
	return packet_get(iface, response, timeout);
}
