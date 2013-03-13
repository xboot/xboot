/*
 * xboot/kernel/command/cmd_xm.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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


#include <xboot.h>
#include <types.h>
#include <string.h>
#include <version.h>
#include <malloc.h>
#include <xboot/log.h>
#include <xboot/printk.h>
#include <xboot/machine.h>
#include <xboot/initcall.h>
#include <command/command.h>
#include <fs/fileio.h>

#if 0

#if	defined(CONFIG_COMMAND_XMODEM) && (CONFIG_COMMAND_XMODEM > 0)

/*
 * FIXME, utf8 console
 */

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

static inline u8_t cksum(u8_t * buf, s32_t size)
{
	s32_t i;
	u8_t sum = 0;

	for(i = 0; i < size; i++)
	{
		sum += buf[i];
	}

	return sum;
}

static inline bool_t check_packet(s32_t crc_flag, u8_t * buf, s32_t size)
{
	u16_t pkt_crc = 0;

	if(crc_flag == 0)
	{
		if( cksum(buf, size) == buf[size])
			return TRUE;
		else
			return FALSE;
	}
	else if(crc_flag == 1)
	{
		pkt_crc = ( (u16_t)buf[size]<<8 ) | ( (u16_t)buf[size+1] );
		if(crc_ccitt(0, buf, size) == pkt_crc)
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

static void flush_input(void)
{
	u32_t c;
	while(console_stdin_getcode_with_timeout(&c, XMODEM_TIMEOUT*3/2));
}

static bool_t xmodem_receive(const char * filename, s32_t * size)
{
	s32_t fd;
	u8_t *packet_buf, *p;
	s32_t packet_size = 128;
	u8_t packet_index = 1;
	s32_t i, retry, retrans = XMODEM_RETRY;
	s32_t crc_flag = 0;
	u8_t trychar = CRC;
	u32_t c;

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
				printk("%c", trychar);

			if( console_stdin_getcode_with_timeout(&c, XMODEM_TIMEOUT<<1) )
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
					printk("%c", ACK);
					free(packet_buf);
					close(fd);
					return TRUE;

				case CAN:
					if(console_stdin_getcode_with_timeout(&c, XMODEM_TIMEOUT) && (c==CAN))
					{
						flush_input();
						printk("%c", ACK);
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
		printk("%c", CAN);
		printk("%c", CAN);
		printk("%c", CAN);
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
			if(!console_stdin_getcode_with_timeout(&c, XMODEM_TIMEOUT))
				goto reject;
			*p++ = c;
		}

		if(packet_buf[1] == (u8_t)(~packet_buf[2]) && (packet_buf[1] == packet_index || packet_buf[1] == (u8_t) packet_index - 1) && check_packet(crc_flag, &packet_buf[3], packet_size))
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
				printk("%c", CAN);
				printk("%c", CAN);
				printk("%c", CAN);
				free(packet_buf);
				close(fd);
				unlink(filename);
				return FALSE;
			}
			printk("%c", ACK);
			continue;
		}

reject:
		flush_input();
		printk("%c", NAK);
	}

	return FALSE;
}

static bool_t xmodem_transmit(const char * filename, s32_t * size)
{
	s32_t fd;
	u8_t * packet_buf;
	u8_t * file_buf;
	s32_t packet_size;
	u8_t packet_index = 1;
	s32_t count;
	s32_t i, retry;
	s32_t crc_flag = -1;
	u32_t c;

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
			if(console_stdin_getcode_with_timeout(&c, XMODEM_TIMEOUT<<1))
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
					if(console_stdin_getcode_with_timeout(&c, XMODEM_TIMEOUT) && (c == CAN))
					{
						printk("%c", ACK);
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
		printk("%c", CAN);
		printk("%c", CAN);
		printk("%c", CAN);
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
					u16_t ccrc = crc_ccitt(0, &packet_buf[3], packet_size);
					packet_buf[packet_size+3] = (ccrc >> 8) & 0xff;
					packet_buf[packet_size+4] = ccrc & 0xff;
				}
				else
				{
					u8_t ccks = 0;
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
						printk("%c", packet_buf[i]);
					}

					if(console_stdin_getcode_with_timeout(&c, XMODEM_TIMEOUT))
					{
						switch(c)
						{
						case ACK:
							packet_index++;
							*size += packet_size;
							goto start_trans;

						case CAN:
							if(console_stdin_getcode_with_timeout(&c, XMODEM_TIMEOUT) && (c == CAN))
							{
								printk("%c", ACK);
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
				printk("%c", CAN);
				printk("%c", CAN);
				printk("%c", CAN);
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
					printk("%c", EOT);
					if(console_stdin_getcode_with_timeout(&c, XMODEM_TIMEOUT<<1) && (c == ACK))
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

static int sx(int argc, char ** argv)
{
	s32_t size;

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

static int rx(int argc, char ** argv)
{
	s32_t size;

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
		LOG_E("register 'sx' command fail");
	if(!command_register(&rx_cmd))
		LOG_E("register 'rx' command fail");
}

static __exit void xmodem_cmd_exit(void)
{
	if(!command_unregister(&sx_cmd))
		LOG_E("unregister 'sx' command fail");
	if(!command_unregister(&rx_cmd))
		LOG_E("unregister 'rx' command fail");
}

command_initcall(xmodem_cmd_init);
command_exitcall(xmodem_cmd_exit);

#endif
#endif
