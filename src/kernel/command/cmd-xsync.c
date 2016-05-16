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

enum xsync_command_t {
	XSYNC_COMMAND_ALIVE		= 0x00,
	XSYNC_COMMAND_START		= 0x01,
	XSYNC_COMMAND_TRANSFER	= 0x02,
	XSYNC_COMMAND_STOP		= 0x03,
	XSYNC_COMMAND_RUN		= 0x04,
	XSYNC_COMMAND_UNKOWN	= 0xff,
};

enum packet_state_t {
	PACKET_STATE_HEADER0,
	PACKET_STATE_HEADER1,
	PACKET_STATE_LENGTH0,
	PACKET_STATE_LENGTH1,
	PACKET_STATE_COMMAND,
	PACKET_STATE_DATA,
	PACKET_STATE_CRC,
};

struct packet_t {
	uint8_t header0;
	uint8_t header1;
	uint8_t length[2];
	uint8_t command;
	uint8_t data[4096];
	uint8_t crc[4];
};

struct xsync_ctx_t {
	struct packet_t packet;
	enum packet_state_t state;
	int index;
};

static const uint32_t crc32_table[256] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106,	0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d,	0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950,	0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7,	0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa,	0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81,	0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84,	0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb,	0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e,	0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55,	0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28,	0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f,	0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242,	0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69,	0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc,	0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693,	0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};

static uint32_t crc32(uint32_t crc, const uint8_t * buf, size_t len)
{
	crc = crc ^ 0xffffffff;

	while(len >= 8)
	{
		crc = crc32_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
		crc = crc32_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
		crc = crc32_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
		crc = crc32_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
		crc = crc32_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
		crc = crc32_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
		crc = crc32_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
		crc = crc32_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
		len -= 8;
	}

	if(len)
	{
		do {
			crc = crc32_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
		} while(--len);
	}

	return crc ^ 0xffffffff;
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
		crc = crc32(crc, &(packet->header0), 1);
		crc = crc32(crc, &(packet->header1), 1);
		crc = crc32(crc, &(packet->length[0]), 2);
		crc = crc32(crc, &(packet->command), 1);
		crc = crc32(crc, &(packet->data[0]), packet_dsize(packet));
	}
	return crc;
}

static void packet_init(struct packet_t * packet, uint8_t command, uint8_t * data, size_t size)
{
	uint32_t crc;

	if(!data)
		size = 0;
	packet->header0 = 'X';
	packet->header1 = 'x';
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

static void packet_put(struct packet_t * packet)
{
	uint16_t dsize = packet_dsize(packet);
	int i;

	putchar(packet->header0);
	putchar(packet->header1);
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
	putchar(packet->crc[0]);
	putchar(packet->crc[1]);
	putchar(packet->crc[2]);
	putchar(packet->crc[3]);
	fflush(stdout);
}

static void xsync_put(uint8_t command, uint8_t * data, size_t size)
{
	struct packet_t packet;

	packet_init(&packet, command, data, size);
	packet_put(&packet);
}

static int xsync_get(struct xsync_ctx_t * ctx, uint8_t c)
{
	uint8_t * p = (uint8_t *)(&ctx->packet);
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
			ctx->state = PACKET_STATE_CRC;
		break;

	case PACKET_STATE_DATA:
		length = (p[2] << 8) | (p[3] << 0);
		if(ctx->index >= length)
			ctx->state = PACKET_STATE_CRC;
		break;

	case PACKET_STATE_CRC:
		length = (p[2] << 8) | (p[3] << 0);
		switch(ctx->index - length - 1)
		{
		case 0:
			ctx->packet.crc[0] = c;
			break;
		case 1:
			ctx->packet.crc[1] = c;
			break;
		case 2:
			ctx->packet.crc[2] = c;
			break;
		case 3:
			ctx->packet.crc[3] = c;
			ctx->index = 0;
			ctx->state = PACKET_STATE_HEADER0;
			crc  = (ctx->packet.crc[0] << 24) & 0xff000000;
			crc |= (ctx->packet.crc[1] << 16) & 0x00ff0000;
			crc |= (ctx->packet.crc[2] <<  8) & 0x0000ff00;
			crc |= (ctx->packet.crc[3] <<  0) & 0x000000ff;
			if(packet_crc(&ctx->packet) == crc)
				return 0;
			break;
		default:
			ctx->index = 0;
			ctx->state = PACKET_STATE_HEADER0;
			break;
		}
		break;

	default:
		ctx->index = 0;
		ctx->state = PACKET_STATE_HEADER0;
		break;
	}

	return -1;
}

static void xsync_handle(struct xsync_ctx_t * ctx)
{
	uint8_t buf[4096];

	switch(ctx->packet.command)
	{
	case XSYNC_COMMAND_ALIVE:
		xsync_put(XSYNC_COMMAND_ALIVE, 0, 0);
		break;

	case XSYNC_COMMAND_START:
	case XSYNC_COMMAND_TRANSFER:
	case XSYNC_COMMAND_STOP:
		break;

	case XSYNC_COMMAND_RUN:
		xsync_put(XSYNC_COMMAND_RUN, 0, 0);
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

	ctx.state = PACKET_STATE_HEADER0;
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
