/*
 * kernel/command/cmd-xsync.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <shell/system.h>
#include <command/command.h>

enum xsync_state_t {
	XSYNC_STATE_HEADER1	= 0,
	XSYNC_STATE_HEADER2	= 1,
	XSYNC_STATE_LENGTH1	= 2,
	XSYNC_STATE_LENGTH2	= 3,
	XSYNC_STATE_COMMAND	= 4,
	XSYNC_STATE_DATA	= 5,
	XSYNC_STATE_CRC		= 6,
};

enum xsync_command_t {
	XSYNC_COMMAND_UNKOWN	= 0x00,
	XSYNC_COMMAND_EXEC		= 0xff,
};

struct xsync_packet_t {
	uint8_t header1;
	uint8_t header2;
	uint8_t length[2];
	uint8_t command;
	uint8_t data[4096];
	uint8_t crc;
};

struct xsync_ctx_t {
	enum xsync_state_t state;
	struct xsync_packet_t packet;
	int index;
};

static const uint8_t crc8_table[256] = {
	0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83,
	0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
	0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e,
	0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
	0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0,
	0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
	0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d,
	0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
	0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5,
	0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
	0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58,
	0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
	0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6,
	0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
	0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b,
	0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
	0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f,
	0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
	0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92,
	0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
	0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c,
	0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
	0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1,
	0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
	0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49,
	0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
	0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4,
	0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
	0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a,
	0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
	0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7,
	0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};

static inline uint8_t crc8_byte(uint8_t crc, const uint8_t data)
{
	return crc8_table[crc ^ data];
}

static uint8_t crc8(uint8_t crc, const uint8_t * buf, size_t len)
{
	while(len--)
		crc = crc8_byte(crc, *buf++);
	return crc;
}

static uint16_t packet_length(struct xsync_packet_t * packet)
{
	return ((packet->length[0] << 8) | (packet->length[1] << 0));
}

static uint16_t packet_dsize(struct xsync_packet_t * packet)
{
	return packet_length(packet) - 2;
}

static uint8_t packet_mkcrc(struct xsync_packet_t * packet)
{
	uint8_t crc = 0;

	if(packet)
	{
		crc = crc8(crc, &(packet->header1), 1);
		crc = crc8(crc, &(packet->header2), 1);
		crc = crc8(crc, &(packet->length[0]), 2);
		crc = crc8(crc, &(packet->command), 1);
		crc = crc8(crc, &(packet->data[0]), packet_dsize(packet));
	}
	return crc;
}

static void packet_init(struct xsync_packet_t * packet, uint8_t command, uint8_t * data, size_t size)
{
	uint16_t length;

	if(!data)
		size = 0;
	length = size + 2;

	packet->header1 = 0x58;
	packet->header2 = 0x78;
	packet->length[0] = (length >> 8) & 0xff;
	packet->length[1] = (length >> 0) & 0xff;
	packet->command = command;
	if(length > 2)
		memcpy(&(packet->data[0]), data, length - 2);
	packet->crc = packet_mkcrc(packet);
}

static void packet_put(struct xsync_packet_t * packet)
{
	uint16_t dsize = packet_dsize(packet);
	int i;

	putchar(packet->header1);
	putchar(packet->header2);
	putchar(packet->length[0]);
	putchar(packet->length[1]);
	putchar(packet->command);
	for(i = 0; i < dsize; i++)
	{
		putchar(packet->data[i]);
		if((i & 0x7) == 0)
		{
			udelay(200);
			fflush(stdout);
		}
	}
	putchar(packet->crc);
	fflush(stdout);
}

static void xsync_put(uint8_t command, uint8_t * data, size_t size)
{
	struct xsync_packet_t packet;

	packet_init(&packet, command, data, size);
	packet_put(&packet);
}

static int xsync_get(struct xsync_ctx_t * ctx, uint8_t c)
{
	uint8_t * p = (uint8_t *)(&ctx->packet);
	uint16_t length;

	p[ctx->index++] = c;

	switch(ctx->state)
	{
	case XSYNC_STATE_HEADER1:
		if(c == 0x58)
		{
			ctx->state = XSYNC_STATE_HEADER2;
		}
		else
		{
			ctx->index = 0;
			ctx->state = XSYNC_STATE_HEADER1;
		}
		break;

	case XSYNC_STATE_HEADER2:
		if(c == 0x78)
		{
			ctx->state = XSYNC_STATE_LENGTH1;
		}
		else
		{
			ctx->index = 0;
			ctx->state = XSYNC_STATE_HEADER1;
		}
		break;

	case XSYNC_STATE_LENGTH1:
		ctx->state = XSYNC_STATE_LENGTH2;
		break;

	case XSYNC_STATE_LENGTH2:
		ctx->state = XSYNC_STATE_COMMAND;
		break;

	case XSYNC_STATE_COMMAND:
		ctx->state = XSYNC_STATE_DATA;
		length = (p[2] << 8) | (p[3] << 0);
		if(ctx->index >= length + 3)
			ctx->state = XSYNC_STATE_CRC;
		break;

	case XSYNC_STATE_DATA:
		length = (p[2] << 8) | (p[3] << 0);
		if(ctx->index >= length + 3)
			ctx->state = XSYNC_STATE_CRC;
		break;

	case XSYNC_STATE_CRC:
		length = (p[2] << 8) | (p[3] << 0);
		ctx->index = 0;
		ctx->state = XSYNC_STATE_HEADER1;
		if(packet_mkcrc(&ctx->packet) == c)
			return 0;
		break;

	default:
		ctx->index = 0;
		ctx->state = XSYNC_STATE_HEADER1;
		break;
	}

	return -1;
}

static void xsync_handle(struct xsync_ctx_t * ctx)
{
	uint8_t buf[1024];

	switch(ctx->packet.command)
	{
	case XSYNC_COMMAND_EXEC:
		xsync_put(XSYNC_COMMAND_EXEC, 0, 0);
		strlcpy((char *)buf, (const char *)ctx->packet.data, packet_dsize(&ctx->packet));
		system((const char *)buf);
		break;

	default:
		xsync_put(XSYNC_COMMAND_UNKOWN, 0, 0);
		break;
	}
}

static void usage(void)
{
	printf("usage:\r\n");
	printf("    xsync\r\n");
}

static int do_xsync(int argc, char ** argv)
{
	struct xsync_ctx_t ctx;
	int c;

	ctx.state = XSYNC_STATE_HEADER1;
	ctx.index = 0;

	while(1)
	{
		if((c = getchar()) < 0)
			continue;
		if(xsync_get(&ctx, c) < 0)
			continue;
		xsync_handle(&ctx);
	}
	return 0;
}

static struct command_t cmd_xsync = {
	.name	= "xsync",
	.desc	= "xsync server for file copying",
	.usage	= usage,
	.exec	= do_xsync,
};

static __init void xsync_cmd_init(void)
{
	register_command(&cmd_xsync);
}

static __exit void xsync_cmd_exit(void)
{
	unregister_command(&cmd_xsync);
}

command_initcall(xsync_cmd_init);
command_exitcall(xsync_cmd_exit);
