/*
 * kernel/command/cmd-xm.c
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

static const uint16_t crc16_table[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};

static uint16_t xm_crc16(uint16_t crc, const uint8_t * buf, int len)
{
	int i;

	for(i = 0; i < len; i++)
		crc = crc16_table[((crc >> 8) ^ *buf++) & 0xff] ^ (crc << 8);
	return crc;
}

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
			uint16_t crc16 = xm_crc16(0, ctx->buf, ctx->pksz);
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
		return (xm_crc16(0, ctx->buf, ctx->bufsz) == crc16) ? 0 : -1;

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
