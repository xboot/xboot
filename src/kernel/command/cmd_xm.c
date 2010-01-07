/*
 * xboot/kernel/command/cmd_xm.c
 *
 *
 * Copyright (c) 2007-2008  jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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


#include <configs.h>
#include <default.h>
#include <types.h>
#include <debug.h>
#include <string.h>
#include <version.h>
#include <malloc.h>
#include <vsprintf.h>
#include <crc16-ccitt.h>
#include <xboot/scank.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <shell/command.h>
#include <fs/fsapi.h>


#if	defined(CONFIG_COMMAND_XMODEM) && (CONFIG_COMMAND_XMODEM > 0)

#define SOH				0x01	/* start of header */
#define	STX 			0x02	/* start of header */
#define EOT				0x04	/* end of text */
#define ACK				0x06	/* acknowledge */
#define NAK				0x15	/* negative acknowledge */
#define CAN				0x18	/* cancel */
#define CRC				0x43	/* crc acknowledge */
#define CTRLZ			0x1a	/* ctrl-z */

#define XMODEM_RETRY	25
#define	XMODEM_TIMEOUT	1000	/* wait one second */

static inline x_u8 cksum(x_u8 * buf, x_s32 size)
{
	x_s32 i;
	x_u8 sum = 0;

	for(i = 0; i < size; i++)
	{
		sum += buf[i];
	}

	return sum;
}

static inline x_bool check_packet(x_s32 crc_flag, x_u8 * buf, x_s32 size)
{
	x_u16 pkt_crc = 0;

	if(crc_flag == 0)
	{
		if( cksum(buf, size) == buf[size])
			return TRUE;
		else
			return FALSE;
	}
	else if(crc_flag == 1)
	{
		pkt_crc = ( (x_u16)buf[size]<<8 ) | ( (x_u16)buf[size+1] );
		if(crc16_ccitt(buf, size) == pkt_crc)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

static void flush_input(void)
{
	char c;
	while(getch_with_timeout(&c, XMODEM_TIMEOUT*3/2));
}

static x_bool xmodem_receive(const char * filename, x_s32 * size)
{
	x_s32 fd;
	x_u8 *packet_buf, *p;
	x_s32 packet_size = 128;
	x_u8 packet_index = 1;
	x_s32 i, retry, retrans = XMODEM_RETRY;
	x_s32 crc_flag = 0;
	x_u8 trychar = CRC;
	char c;

	if(!filename || !size)
		return FALSE;

	/* 1024 for xmodem 1k + 3 head chars + 2 crc + null */
	packet_buf = malloc(1024 + 6);
	if(!packet_buf)
		return FALSE;

	*size = 0;

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
	if(fd < 0)
	{
		free(packet_buf);
		printk("can not to open the file '%s'\r\n", filename);
		return FALSE;
	}

	for(;;)
	{
		for(retry = 0; retry < 16; retry++)
		{
			if(trychar)
				putch(trychar);

			if( getch_with_timeout(&c, XMODEM_TIMEOUT<<1) )
			{
				switch(c)
				{
				case SOH:
					packet_size = 128;
					goto start_recv;

				case STX:
					packet_size = 1024;
					goto start_recv;

				case EOT:
					flush_input();
					putch(ACK);
					free(packet_buf);
					close(fd);
					return TRUE;

				case CAN:
					if(getch_with_timeout(&c, XMODEM_TIMEOUT) && (c==CAN))
					{
						flush_input();
						putch(ACK);
						free(packet_buf);
						close(fd);
						unlink(filename);
						return FALSE;
					}
					break;

				default:
					break;
				}
			}
		}

		if(trychar == CRC)
		{
			trychar = NAK;
			continue;
		}

		/* sync error */
		flush_input();
		putch(CAN);
		putch(CAN);
		putch(CAN);
		free(packet_buf);
		close(fd);
		unlink(filename);
		return FALSE;

start_recv:
		if (trychar == CRC)
			crc_flag = 1;
		trychar = 0;
		p = packet_buf;
		*p++ = c;
		for(i = 0; i < (packet_size + (crc_flag ? 1 : 0) + 3); ++i)
		{
			if(!getch_with_timeout(&c, XMODEM_TIMEOUT))
				goto reject;
			*p++ = c;
		}

		if(packet_buf[1] == (x_u8)(~packet_buf[2]) && (packet_buf[1] == packet_index || packet_buf[1] == (x_u8) packet_index - 1) && check_packet(crc_flag, &packet_buf[3], packet_size))
		{
			if(packet_buf[1] == packet_index)
			{
				write(fd, &packet_buf[3], packet_size);
				*size += packet_size;
				++packet_index;
				retrans = XMODEM_RETRY + 1;
			}

			/* too many retry error */
			if(--retrans <= 0)
			{
				flush_input();
				putch(CAN);
				putch(CAN);
				putch(CAN);
				free(packet_buf);
				close(fd);
				unlink(filename);
				return FALSE;
			}
			putch(ACK);
			continue;
		}

reject:
		flush_input();
		putch(NAK);
	}

	return FALSE;
}

static x_bool xmodem_transmit(const char * filename, x_s32 * size)
{
	x_s32 fd;
	x_u8 * packet_buf;
	x_u8 * file_buf;
	x_s32 packet_size;
	x_u8 packet_index = 1;
	x_s32 count;
	x_s32 i, retry;
	x_s32 crc_flag = -1;
	char c;

	if(!filename || !size)
		return FALSE;

	packet_buf = malloc(1024 + 6);
	if(!packet_buf)
		return FALSE;

	file_buf = malloc(1024);
	if(!file_buf)
		return FALSE;

	*size = 0;

	fd = open(filename, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
	{
		free(file_buf);
		free(packet_buf);
		printk("can not to open the file '%s'\r\n", filename);
		return FALSE;
	}

	for(;;)
	{
		for(retry = 0; retry < 16; retry++)
		{
			if(getch_with_timeout(&c, XMODEM_TIMEOUT<<1))
			{
				switch(c)
				{
				case CRC:
					crc_flag = 1;
					goto start_trans;

				case NAK:
					crc_flag = 0;
					goto start_trans;

				case CAN:
					if(getch_with_timeout(&c, XMODEM_TIMEOUT) && (c == CAN))
					{
						putch(ACK);
						flush_input();
						free(file_buf);
						free(packet_buf);
						close(fd);
						return FALSE;
					}
					break;

				default:
					break;
				}
			}
		}

		/* no sync */
		putch(CAN);
		putch(CAN);
		putch(CAN);
		flush_input();
		free(file_buf);
		free(packet_buf);
		close(fd);
		return FALSE;

		for(;;)
		{
start_trans:
			packet_buf[0] = SOH;
			packet_size = 128;
			packet_buf[1] = packet_index;
			packet_buf[2] = ~packet_index;

			count = read(fd, (void *)file_buf, packet_size);
			if(count > 0)
			{
				memset(&packet_buf[3], CTRLZ, packet_size);
				memcpy(&packet_buf[3], file_buf, count);

				if(crc_flag)
				{
					x_u16 ccrc = crc16_ccitt(&packet_buf[3], packet_size);
					packet_buf[packet_size+3] = (ccrc >> 8) & 0xff;
					packet_buf[packet_size+4] = ccrc & 0xff;
				}
				else
				{
					x_u8 ccks = 0;
					for(i = 3; i < packet_size + 3; i++)
					{
						ccks += packet_buf[i];
					}
					packet_buf[packet_size+3] = ccks;
				}

				for(retry = 0; retry < XMODEM_RETRY; retry++)
				{
					for(i = 0; i < packet_size + 4 + (crc_flag ? 1 : 0); i++)
					{
						putch(packet_buf[i]);
					}

					if(getch_with_timeout(&c, XMODEM_TIMEOUT))
					{
						switch(c)
						{
						case ACK:
							packet_index++;
							*size += packet_size;
							goto start_trans;

						case CAN:
							if(getch_with_timeout(&c, XMODEM_TIMEOUT) && (c == CAN))
							{
								putch(ACK);
								flush_input();
								free(file_buf);
								free(packet_buf);
								close(fd);
								return FALSE;
							}
							break;

						case NAK:
							break;

						default:
							break;
						}
					}
				}

				/* xmit error */
				putch(CAN);
				putch(CAN);
				putch(CAN);
				flush_input();
				free(file_buf);
				free(packet_buf);
				close(fd);
				return FALSE;
			}
			else
			{
				for(retry = 0; retry < 10; ++retry)
				{
					putch(EOT);
					if(getch_with_timeout(&c, XMODEM_TIMEOUT<<1) && (c == ACK))
						break;
				}

				flush_input();
				free(file_buf);
				free(packet_buf);
				close(fd);
				if(c == ACK)
					return TRUE;
				else
					return FALSE;
			}
		}
	}

	return FALSE;
}

static x_s32 sx(x_s32 argc, const x_s8 **argv)
{
	x_s32 size;

	if(argc != 2)
	{
		printk("usage:\r\n    sx <filename>\r\n");
		return (-1);
	}

	if(xmodem_transmit((const char *)argv[1], &size))
	{
		printk("transmit complete, the size is %ld bytes\r\n", size);
		return 0;
	}
	else
	{
		printk("transmit fail\r\n");
		return -1;
	}

	return 0;
}

static x_s32 rx(x_s32 argc, const x_s8 **argv)
{
	x_s32 size;

	if(argc != 2)
	{
		printk("usage:\r\n    rx <filename>\r\n");
		return (-1);
	}

	if(xmodem_receive((const char *)argv[1], &size))
	{
		printk("\r\n");
		printk("receive complete, the size is %ld bytes\r\n", size);
		return 0;
	}
	else
	{
		printk("\r\n");
		printk("receive fail\r\n");
		return -1;
	}

	return 0;
}

static struct command sx_cmd = {
	.name		= "sx",
	.func		= sx,
	.desc		= "send file using xmodem\r\n",
	.usage		= "sx <filename>\r\n",
	.help		= "    send file using xmodem transport protocol.\r\n"
};

static struct command rx_cmd = {
	.name		= "rx",
	.func		= rx,
	.desc		= "receive file using xmodem\r\n",
	.usage		= "rx <filename>\r\n",
	.help		= "    receive file using xmodem transport protocol.\r\n"
};

static __init void xmodem_cmd_init(void)
{
	if(!command_register(&sx_cmd))
		DEBUG_E("register 'sx' command fail");
	if(!command_register(&rx_cmd))
		DEBUG_E("register 'rx' command fail");
}

static __exit void xmodem_cmd_exit(void)
{
	if(!command_unregister(&sx_cmd))
		DEBUG_E("unregister 'sx' command fail");
	if(!command_unregister(&rx_cmd))
		DEBUG_E("unregister 'rx' command fail");
}

module_init(xmodem_cmd_init, LEVEL_COMMAND);
module_exit(xmodem_cmd_exit, LEVEL_COMMAND);

#endif
