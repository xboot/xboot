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

enum xm_state_t {
	XM_STATE_CONNECTING	= 0,
	XM_STATE_START		= 1,
	XM_STATE_PKNUM0		= 2,
	XM_STATE_PKNUM1		= 3,
	XM_STATE_BODY		= 4,
	XM_STATE_CRC0		= 5,
	XM_STATE_CRC1		= 6,
};

enum crc_mode_t {
	CRC_MODE_ADD8		= 0,
	CRC_MODE_CRC16		= 1,
};

struct xm_ctx_t {
	enum xm_state_t state;
	enum crc_mode_t mode;
	int fileid;
	int filelen;
	int retry;
	int pksz;
	int pknum;
	int bufsz;
	uint8_t pknum0;
	uint8_t pknum1;
	uint8_t buf[1024];
	uint8_t crc0;
	uint8_t crc1;
};

static const uint16_t crc_ccitt_table[256] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

static inline uint16_t crc_ccitt_byte(uint16_t crc, const uint8_t c)
{
	return (crc >> 8) ^ crc_ccitt_table[(crc ^ c) & 0xff];
}

static uint16_t crc_ccitt(uint16_t crc, const uint8_t * buf, size_t len)
{
	while(len--)
		crc = crc_ccitt_byte(crc, *buf++);
	return crc;
}

static int xm_getch_timeout(int ms)
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

static int xm_check_packet(struct xm_ctx_t * ctx)
{
	uint8_t crc8 = 0;
	uint16_t crc16 = 0;
	int i;

	return 0;

/*	if(ctx->pknum0 != ~ctx->pknum1)
		return -1;
*/
	switch(ctx->mode)
	{
	case CRC_MODE_ADD8:
		for(i = 0; i < ctx->bufsz; i++)
			crc8 += ctx->buf[i];
		return (crc8 == ctx->crc0) ? 0 : -1;

	case CRC_MODE_CRC16:
		crc16 = ((uint16_t)ctx->crc0 << 8) | (uint16_t)ctx->crc1;
		return (crc_ccitt(0, ctx->buf, ctx->bufsz) == crc16) ? 0 : -1;

	default:
		break;
	}

	return -1;
}

static int xm_receive(struct xm_ctx_t * ctx)
{
	int c;

	while(1)
	{
		if(ctx->state == XM_STATE_CONNECTING)
		{
			if(++ctx->retry > 10)
			{
				xm_putch(NAK);
				ctx->mode = CRC_MODE_ADD8;
			}
			else
			{
				xm_putch('C');
				ctx->mode = CRC_MODE_CRC16;
			}
		}
		if((c = xm_getch_timeout(1000)) < 0)
			continue;

		switch(ctx->state)
		{
		case XM_STATE_CONNECTING:
		case XM_STATE_START:
			switch(c)
			{
			case SOH:
				ctx->pksz = 128;
				ctx->state = XM_STATE_PKNUM0;
				break;

			case STX:
				ctx->pksz = 1024;
				ctx->state = XM_STATE_PKNUM0;
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

		case XM_STATE_PKNUM0:
			ctx->pknum0 = c;
			ctx->state = XM_STATE_PKNUM1;
			break;

		case XM_STATE_PKNUM1:
			ctx->pknum1 = c;
			ctx->bufsz = 0;
			ctx->state = XM_STATE_BODY;
			break;

		case XM_STATE_BODY:
			ctx->buf[ctx->bufsz++] = c;
			if(ctx->bufsz >= ctx->pksz)
				ctx->state = XM_STATE_CRC0;
			break;

		case XM_STATE_CRC0:
			ctx->crc0 = c;
			if(ctx->mode == CRC_MODE_ADD8)
			{
				if(xm_check_packet(ctx) < 0)
				{
					xm_putch(NAK);
				}
				else
				{
					ctx->pknum++;
					ctx->filelen += write(ctx->fileid, ctx->buf, ctx->bufsz);
					xm_putch(ACK);
				}
				ctx->state = XM_STATE_START;
			}
			else if(ctx->mode == CRC_MODE_CRC16)
			{
				ctx->state = XM_STATE_CRC1;
			}
			break;

		case XM_STATE_CRC1:
			ctx->crc1 = c;
			if(xm_check_packet(ctx) < 0)
			{
				xm_putch(NAK);
			}
			else
			{
				ctx->pknum++;
				ctx->filelen += write(ctx->fileid, ctx->buf, ctx->bufsz);
				xm_putch(ACK);
			}
			ctx->state = XM_STATE_START;
			break;

		default:
			ctx->state = XM_STATE_START;
			break;
		}
	}
}

static void rx_usage(void)
{
	printf("usage:\r\n");
	printf("    rx <filename>\r\n");
}

static int rx(int argc, char ** argv)
{
	struct xm_ctx_t ctx;
	int fd;

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

	ctx.state = XM_STATE_CONNECTING;
	ctx.mode = CRC_MODE_CRC16;
	ctx.fileid = fd;
	ctx.filelen = 0;
	ctx.retry = 0;
	ctx.pksz = 1024;
	ctx.pknum = 0;
	ctx.bufsz = 0;

	if(xm_receive(&ctx) < 0)
	{
		printf("Receive fail\r\n");
		return -1;
	}
	else
	{
		printf("Receive complete, size is %ld bytes\r\n", ctx.filelen);
		return 0;
	}

	close(fd);
	return 0;
}

static struct command_t cmd_rx = {
	.name	= "rx",
	.desc	= "receive file using xmodem",
	.usage	= rx_usage,
	.exec	= rx,
};

static __init void xm_cmd_init(void)
{
	register_command(&cmd_rx);
}

static __exit void xm_cmd_exit(void)
{
	unregister_command(&cmd_rx);
}

command_initcall(xm_cmd_init);
command_exitcall(xm_cmd_exit);
