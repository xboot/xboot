/*
 * kernel/command/cmd-xm.c
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

#include <crc16.h>
#include <command/command.h>

enum {
	SOH	= 0x01,
	STX	= 0x02,
	EOT	= 0x04,
	ACK	= 0x06,
	NAK	= 0x15,
	CAN	= 0x18,
};

enum crc_mode_t {
	CRC_MODE_ADD8	= 0,
	CRC_MODE_CRC16	= 1,
};

enum xm_send_state_t {
	XM_SEND_STATE_CONNECTING	= 0,
	XM_SEND_STATE_TRANSMIT		= 1,
	XM_SEND_STATE_WAIT_ACK		= 2,
	XM_SEND_STATE_WAIT_EOT		= 3,
};

enum xm_recv_state_t {
	XM_RECV_STATE_CONNECTING	= 0,
	XM_RECV_STATE_START			= 1,
	XM_RECV_STATE_PKNUM0		= 2,
	XM_RECV_STATE_PKNUM1		= 3,
	XM_RECV_STATE_BODY			= 4,
	XM_RECV_STATE_CRC0			= 5,
	XM_RECV_STATE_CRC1			= 6,
};

struct xm_send_ctx_t {
	enum xm_send_state_t state;
	enum crc_mode_t mode;
	int fd;
	int timeout;
	int index;
	int pksz;
	int eot;
	uint8_t start;
	uint8_t num0;
	uint8_t num1;
	uint8_t buf[1024];
	uint8_t crc0;
	uint8_t crc1;
};

struct xm_recv_ctx_t {
	enum xm_recv_state_t state;
	enum crc_mode_t mode;
	int fd;
	int retry;
	int timeout;
	int index;
	int pksz;
	int bufsz;
	uint8_t num0;
	uint8_t num1;
	uint8_t buf[1024];
	uint8_t crc0;
	uint8_t crc1;
};

static int xm_getch(int ms)
{
	ktime_t timeout = ktime_add_ms(ktime_get(), ms);
	int ch;

	do {
		if((ch = getchar()) != EOF)
			return ch;
	} while(ktime_before(ktime_get(), timeout));

	return ch;
}

static void xm_putch(int ch)
{
	putchar(ch);
	fflush(stdout);
}

static void xm_send_fill_packet(struct xm_send_ctx_t * ctx)
{
	int i;

	if(ctx->pksz == 128)
		ctx->start = SOH;
	else if(ctx->pksz == 1024)
		ctx->start = STX;
	ctx->num0 = ctx->index;
	ctx->num1 = 255 - ctx->index;
	memset(ctx->buf, 0x1a, ctx->pksz);
	if(read(ctx->fd, (void *)ctx->buf, ctx->pksz) > 0)
	{
		ctx->eot = 0;
		if(ctx->mode == CRC_MODE_ADD8)
		{
			for(ctx->crc0 = 0, i = 0; i < ctx->pksz; i++)
				ctx->crc0 += ctx->buf[i];
		}
		else if(ctx->mode == CRC_MODE_CRC16)
		{
			uint16_t crc16 = crc16_sum(0, ctx->buf, ctx->pksz);
			ctx->crc0 = (crc16 >> 8) & 0xff;
			ctx->crc1 = crc16 & 0xff;
		}
	}
	else
	{
		ctx->eot = 1;
	}
}

static int xm_recv_verify_packet(struct xm_recv_ctx_t * ctx)
{
	uint8_t crc8 = 0;
	uint16_t crc16 = 0;
	int i;

	if(ctx->num0 != (255 - ctx->num1))
		return -1;

	if((ctx->num0 != (ctx->index & 0xff)) && (ctx->num0 != ((ctx->index - 1) & 0xff)))
		return -1;

	switch(ctx->mode)
	{
	case CRC_MODE_ADD8:
		for(i = 0; i < ctx->bufsz; i++)
			crc8 += ctx->buf[i];
		return (crc8 == ctx->crc0) ? 0 : -1;

	case CRC_MODE_CRC16:
		crc16 = ((uint16_t)ctx->crc0 << 8) | (uint16_t)ctx->crc1;
		return (crc16_sum(0, ctx->buf, ctx->bufsz) == crc16) ? 0 : -1;

	default:
		break;
	}

	return -1;
}

static int xm_send(struct xm_send_ctx_t * ctx)
{
	int c, i;

	while(1)
	{
		if((c = xm_getch(1000)) < 0)
		{
			if((ctx->state == XM_SEND_STATE_WAIT_ACK) || (ctx->state == XM_SEND_STATE_WAIT_EOT))
			{
				if(++ctx->timeout > 3)
				{
					xm_putch(CAN);
					xm_putch(CAN);
					xm_putch(CAN);
					return -1;
				}
			}
			continue;
		}

		switch(ctx->state)
		{
		case XM_SEND_STATE_CONNECTING:
			switch(c)
			{
			case 'C':
				ctx->mode = CRC_MODE_CRC16;
				xm_send_fill_packet(ctx);
				ctx->state = XM_SEND_STATE_TRANSMIT;
				break;

			case NAK:
				ctx->mode = CRC_MODE_ADD8;
				xm_send_fill_packet(ctx);
				ctx->state = XM_SEND_STATE_TRANSMIT;
				break;

			case CAN:
				return -1;

			case 0x3:
				return -1;

			default:
				break;
			}
			break;

		case XM_SEND_STATE_WAIT_ACK:
			switch(c)
			{
			case ACK:
				ctx->index = (ctx->index + 1) & 0xff;
				xm_send_fill_packet(ctx);
				ctx->state = XM_SEND_STATE_TRANSMIT;
				break;

			case NAK:
				ctx->state = XM_SEND_STATE_TRANSMIT;
				break;

			case CAN:
				return -1;

			default:
				break;
			}
			break;

		case XM_SEND_STATE_WAIT_EOT:
			switch(c)
			{
			case ACK:
				return 0;

			case NAK:
				ctx->state = XM_SEND_STATE_TRANSMIT;
				break;

			case CAN:
				return -1;

			default:
				break;
			}
			break;

		default:
			break;
		}

		if(ctx->state == XM_SEND_STATE_TRANSMIT)
		{
			if(ctx->eot)
			{
				xm_putch(EOT);
				ctx->timeout = 0;
				ctx->state = XM_SEND_STATE_WAIT_EOT;
			}
			else
			{
				xm_putch(ctx->start);
				xm_putch(ctx->num0);
				xm_putch(ctx->num1);
				for(i = 0; i < ctx->pksz; i++)
					xm_putch(ctx->buf[i]);
				xm_putch(ctx->crc0);
				if(ctx->mode == CRC_MODE_CRC16)
					xm_putch(ctx->crc1);
				ctx->timeout = 0;
				ctx->state = XM_SEND_STATE_WAIT_ACK;
			}
		}
	}
}

static int xm_recv(struct xm_recv_ctx_t * ctx)
{
	int c;

	while(1)
	{
		if((c = xm_getch(1000)) < 0)
		{
			if(ctx->state == XM_RECV_STATE_CONNECTING)
			{
				if(++ctx->retry < 10)
				{
					xm_putch('C');
					ctx->mode = CRC_MODE_CRC16;
				}
				else
				{
					xm_putch(NAK);
					ctx->mode = CRC_MODE_ADD8;
				}
			}
			else
			{
				if(++ctx->timeout > 3)
				{
					xm_putch(CAN);
					xm_putch(CAN);
					xm_putch(CAN);
					return -1;
				}
			}
			continue;
		}

		switch(ctx->state)
		{
		case XM_RECV_STATE_CONNECTING:
			switch(c)
			{
			case SOH:
				ctx->retry = 0;
				ctx->timeout = 0;
				ctx->pksz = 128;
				ctx->bufsz = 0;
				ctx->state = XM_RECV_STATE_PKNUM0;
				break;

			case STX:
				ctx->retry = 0;
				ctx->timeout = 0;
				ctx->pksz = 1024;
				ctx->bufsz = 0;
				ctx->state = XM_RECV_STATE_PKNUM0;
				break;

			case CAN:
				xm_putch(ACK);
				return -1;

			case 0x3:
				return -1;

			default:
				break;
			}
			break;

		case XM_RECV_STATE_START:
			switch(c)
			{
			case SOH:
				ctx->pksz = 128;
				ctx->state = XM_RECV_STATE_PKNUM0;
				break;

			case STX:
				ctx->pksz = 1024;
				ctx->state = XM_RECV_STATE_PKNUM0;
				break;

			case EOT:
				xm_putch(ACK);
				return 0;

			case CAN:
				xm_putch(ACK);
				return -1;

			default:
				break;
			}
			break;

		case XM_RECV_STATE_PKNUM0:
			ctx->num0 = c;
			ctx->state = XM_RECV_STATE_PKNUM1;
			break;

		case XM_RECV_STATE_PKNUM1:
			ctx->num1 = c;
			ctx->state = XM_RECV_STATE_BODY;
			break;

		case XM_RECV_STATE_BODY:
			ctx->buf[ctx->bufsz++] = c;
			if(ctx->bufsz >= ctx->pksz)
				ctx->state = XM_RECV_STATE_CRC0;
			break;

		case XM_RECV_STATE_CRC0:
			ctx->crc0 = c;
			if(ctx->mode == CRC_MODE_ADD8)
			{
				if(xm_recv_verify_packet(ctx) >= 0)
				{
					if(ctx->num0 == (ctx->index & 0xff))
					{
						write(ctx->fd, ctx->buf, ctx->bufsz);
						ctx->index = (ctx->index + 1) & 0xff;
					}
					ctx->retry = 0;
					ctx->timeout = 0;
					ctx->bufsz = 0;
					xm_putch(ACK);
				}
				else
				{
					if(++ctx->retry < 10)
					{
						ctx->bufsz = 0;
						xm_putch(NAK);
					}
					else
					{
						xm_putch(CAN);
						xm_putch(CAN);
						xm_putch(CAN);
						return -1;
					}
				}
				ctx->state = XM_RECV_STATE_START;
			}
			else if(ctx->mode == CRC_MODE_CRC16)
			{
				ctx->state = XM_RECV_STATE_CRC1;
			}
			break;

		case XM_RECV_STATE_CRC1:
			ctx->crc1 = c;
			if(xm_recv_verify_packet(ctx) >= 0)
			{
				if(ctx->num0 == (ctx->index & 0xff))
				{
					write(ctx->fd, ctx->buf, ctx->bufsz);
					ctx->index = (ctx->index + 1) & 0xff;
				}
				ctx->retry = 0;
				ctx->timeout = 0;
				ctx->bufsz = 0;
				xm_putch(ACK);
			}
			else
			{
				if(++ctx->retry < 10)
				{
					ctx->bufsz = 0;
					xm_putch(NAK);
				}
				else
				{
					xm_putch(CAN);
					xm_putch(CAN);
					xm_putch(CAN);
					return -1;
				}
			}
			ctx->state = XM_RECV_STATE_START;
			break;

		default:
			ctx->state = XM_RECV_STATE_START;
			break;
		}
	}
}

static void sx_usage(void)
{
	printf("usage:\r\n");
	printf("    sx [-k] <filename>\r\n");
}

static void rx_usage(void)
{
	printf("usage:\r\n");
	printf("    rx <filename>\r\n");
}

static int sx(int argc, char ** argv)
{
	struct xm_send_ctx_t ctx;
	char * filename = "";
	int i, k = 0;
	int fd, ret;

	if(argc < 2 || argc > 3)
	{
		sx_usage();
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-k"))
			k = 1;
		else
			filename = argv[i];
	}

	fd = open(filename, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
	{
		printf("Can not to open file '%s'\r\n", argv[1]);
		return -1;
	}

	ctx.state = XM_SEND_STATE_CONNECTING;
	ctx.fd = fd;
	ctx.timeout = 0;
	ctx.index = 1;
	ctx.pksz = (k == 0) ? 128 : 1024;

	ret = xm_send(&ctx);
	close(fd);
	return ret;
}

static int rx(int argc, char ** argv)
{
	struct xm_recv_ctx_t ctx;
	int fd, ret;

	if(argc != 2)
	{
		rx_usage();
		return -1;
	}

	fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
	if(fd < 0)
	{
		printf("Can not to open file '%s'\r\n", argv[1]);
		return -1;
	}

	ctx.state = XM_RECV_STATE_CONNECTING;
	ctx.fd = fd;
	ctx.retry = 0;
	ctx.timeout = 0;
	ctx.bufsz = 0;
	ctx.index = 1;

	ret = xm_recv(&ctx);
	close(fd);
	if(ret < 0)
		unlink(argv[1]);
	return ret;
}

static struct command_t cmd_sx = {
	.name	= "sx",
	.desc	= "send file using xmodem",
	.usage	= sx_usage,
	.exec	= sx,
};

static struct command_t cmd_rx = {
	.name	= "rx",
	.desc	= "receive file using xmodem",
	.usage	= rx_usage,
	.exec	= rx,
};

static __init void xm_cmd_init(void)
{
	register_command(&cmd_sx);
	register_command(&cmd_rx);
}

static __exit void xm_cmd_exit(void)
{
	unregister_command(&cmd_sx);
	unregister_command(&cmd_rx);
}

command_initcall(xm_cmd_init);
command_exitcall(xm_cmd_exit);
