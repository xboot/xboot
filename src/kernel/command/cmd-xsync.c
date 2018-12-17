/*
 * kernel/command/cmd-xsync.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <crc32.h>
#include <command/command.h>

#define PACKET_DATA_MAX		(1024)

enum xsync_command_t {
	XSYNC_COMMAND_ALIVE		= 0x00,
	XSYNC_COMMAND_START		= 0x01,
	XSYNC_COMMAND_TRANSFER	= 0x02,
	XSYNC_COMMAND_STOP		= 0x03,
	XSYNC_COMMAND_SYSTEM	= 0x04,
	XSYNC_COMMAND_UNKOWN	= 0xff,
};

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

struct packet_t {
	uint8_t header[2];
	uint8_t length[2];
	uint8_t command;
	uint8_t data[4096];
	uint8_t crc[4];
};

struct xsync_ctx_t {
	struct packet_t packet;
	enum packet_state_t state;
	int index;
	int fd;
	int quit;
};

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
		crc = crc32_sum(crc, &(packet->header[0]), 2);
		crc = crc32_sum(crc, &(packet->length[0]), 2);
		crc = crc32_sum(crc, &(packet->command), 1);
		crc = crc32_sum(crc, &(packet->data[0]), packet_dsize(packet));
	}
	return crc;
}

static void packet_init(struct packet_t * packet, uint8_t command, uint8_t * data, size_t size)
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

static void packet_put(struct packet_t * packet)
{
	uint16_t dsize = packet_dsize(packet);
	int i;

	putchar(packet->header[0]);
	putchar(packet->header[1]);
	putchar(packet->length[0]);
	putchar(packet->length[1]);
	putchar(packet->command);
	fflush(stdout);
	for(i = 0; i < dsize; i++)
	{
		putchar(packet->data[i]);
		fflush(stdout);
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
			ctx->state = PACKET_STATE_CRC0;
		break;

	case PACKET_STATE_DATA:
		length = (p[2] << 8) | (p[3] << 0);
		if(ctx->index >= length)
			ctx->state = PACKET_STATE_CRC0;
		break;

	case PACKET_STATE_CRC0:
		ctx->packet.crc[0] = c;
		ctx->state = PACKET_STATE_CRC1;
		break;

	case PACKET_STATE_CRC1:
		ctx->packet.crc[1] = c;
		ctx->state = PACKET_STATE_CRC2;
		break;

	case PACKET_STATE_CRC2:
		ctx->packet.crc[2] = c;
		ctx->state = PACKET_STATE_CRC3;
		break;

	case PACKET_STATE_CRC3:
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

	return -1;
}

static uint8_t xsync_handle_start(struct xsync_ctx_t * ctx)
{
	char fpath[VFS_MAX_PATH];
	char path[PACKET_DATA_MAX];
	char buf[PACKET_DATA_MAX];
	uint32_t crc1, crc2 = 0;
	u64_t n;

	crc1  = (ctx->packet.data[0] << 24) & 0xff000000;
	crc1 |= (ctx->packet.data[1] << 16) & 0x00ff0000;
	crc1 |= (ctx->packet.data[2] <<  8) & 0x0000ff00;
	crc1 |= (ctx->packet.data[3] <<  0) & 0x000000ff;
	memset(path, 0, sizeof(path));
	memcpy(path, &ctx->packet.data[4], packet_dsize(&ctx->packet) - 4);

	shell_realpath(path, fpath);
	ctx->fd = vfs_open(fpath, O_RDONLY, 0);
	if(ctx->fd > 0)
	{
		while((n = vfs_read(ctx->fd, buf, sizeof(buf))) > 0)
		{
			crc2 = crc32_sum(crc2, (const uint8_t *)buf, n);
		}
		vfs_close(ctx->fd);
		ctx->fd = -1;

		if((crc1 == crc2) && (crc2 != 0))
			return 1;
	}

	ctx->fd = vfs_open(fpath, O_WRONLY | O_CREAT | O_TRUNC, 0755);
	if(ctx->fd < 0)
		return 0;
	return 2;
}

static void xsync_handle(struct xsync_ctx_t * ctx)
{
	uint8_t buf[PACKET_DATA_MAX];
	size_t size;

	switch(ctx->packet.command)
	{
	case XSYNC_COMMAND_ALIVE:
		size = sprintf((char *)buf, "%s", machine_uniqueid());
		xsync_put(XSYNC_COMMAND_ALIVE, buf, size);
		break;

	case XSYNC_COMMAND_START:
		buf[0] = xsync_handle_start(ctx);
		xsync_put(XSYNC_COMMAND_START, buf, 1);
		break;

	case XSYNC_COMMAND_TRANSFER:
		vfs_write(ctx->fd, (void *)ctx->packet.data, packet_dsize(&ctx->packet));
		xsync_put(XSYNC_COMMAND_TRANSFER, 0, 0);
		break;

	case XSYNC_COMMAND_STOP:
		if(ctx->fd > 0)
		{
			vfs_close(ctx->fd);
			ctx->fd = -1;
		}
		xsync_put(XSYNC_COMMAND_STOP, 0, 0);
		break;

	case XSYNC_COMMAND_SYSTEM:
		xsync_put(XSYNC_COMMAND_SYSTEM, 0, 0);
		ctx->quit = 1;
		if(ctx->fd > 0)
		{
			vfs_close(ctx->fd);
			ctx->fd = -1;
		}
		memset(buf, 0, sizeof(buf));
		memcpy(buf, &ctx->packet.data[0], packet_dsize(&ctx->packet));
		shell_system((const char *)buf);
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
	ktime_t timeout = ktime_add_ms(ktime_get(), 3000);
	int c;

	ctx.state = PACKET_STATE_HEADER0;
	ctx.index = 0;
	ctx.fd = -1;
	ctx.quit = 0;

	while(ctx.quit == 0)
	{
		if((c = getchar()) < 0)
		{
			if(ktime_after(ktime_get(), timeout))
			{
				ctx.quit = 1;
				if(ctx.fd > 0)
				{
					vfs_close(ctx.fd);
					ctx.fd = -1;
				}
			}
			continue;
		}

		if(xsync_get(&ctx, c) < 0)
			continue;

		xsync_handle(&ctx);
		timeout = ktime_add_ms(ktime_get(), 3000);
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
