/*
 * kernel/command/cmd_serial.c
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
#include <ctype.h>
#include <malloc.h>
#include <xboot/log.h>
#include <xboot/ioctl.h>
#include <xboot/list.h>
#include <xboot/chrdev.h>
#include <xboot/device.h>
#include <shell/ctrlc.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <serial/serial.h>
#include <command/command.h>

#if	defined(CONFIG_COMMAND_SERIAL) && (CONFIG_COMMAND_SERIAL > 0)

extern struct device_list * device_list;

static void serial_info(void)
{
	struct chrdev * dev;
	struct device_list * list;
	struct list_head * pos;
	struct serial_info * serial_info;

	for(pos = (&device_list->entry)->next; pos != (&device_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct device_list, entry);
		if(list->device->type != CHAR_DEVICE)
			continue;

		dev = (struct chrdev *)(list->device->priv);
		if(dev->type != CHR_DEV_SERIAL)
			continue;

		printk(" \"%s\" - <", dev->name);

		serial_info = ((struct serial_driver *)(dev->driver))->info;
		switch(serial_info->parameter->baud_rate)
		{
		case B50:
			printk("50,");		break;
		case B75:
			printk("75,");		break;
		case B110:
			printk("110,");		break;
		case B134:
			printk("134,");		break;
		case B200:
			printk("200,");		break;
		case B300:
			printk("300,");		break;
		case B600:
			printk("600,");		break;
		case B1200:
			printk("1200,");	break;
		case B1800:
			printk("1800,");	break;
		case B2400:
			printk("2400,");	break;
		case B4800:
			printk("4800,");	break;
		case B9600:
			printk("9600,");	break;
		case B19200:
			printk("19200,");	break;
		case B38400:
			printk("38400,");	break;
		case B57600:
			printk("57600,");	break;
		case B76800:
			printk("76800,");	break;
		case B115200:
			printk("115200,");	break;
		case B230400:
			printk("230400,");	break;
		case B380400:
			printk("380400,");	break;
		case B460800:
			printk("460800,");	break;
		case B921600:
			printk("921600,");	break;
		default:
			break;
		}

		switch(serial_info->parameter->data_bit)
		{
		case DATA_BITS_5:
			printk("5,");		break;
		case DATA_BITS_6:
			printk("6,");		break;
		case DATA_BITS_7:
			printk("7,");		break;
		case DATA_BITS_8:
			printk("8,");		break;
		default:
			break;
		}

		switch(serial_info->parameter->parity)
		{
		case PARITY_NONE:
			printk("N,");
			break;

		case PARITY_EVEN:
			printk("E,");
			break;

		case PARITY_ODD:
			printk("O,");
			break;

		default: break;
		}

		switch(serial_info->parameter->stop_bit)
		{
		case STOP_BITS_1:
			printk("1>");
			break;

		case STOP_BITS_1_5:
			printk("1.5>");
			break;

		case STOP_BITS_2:
			printk("2>");
			break;

		default: break;
		}
		printk(" - %s\r\n", serial_info->desc);
	}
}

static int serial(int argc, char ** argv)
{
	struct chrdev * device = NULL;
	ssize_t (* read_func)(struct chrdev *, u8_t *, size_t);
	struct serial_parameter param;
	char * name = NULL, * str = NULL;
	char * baud = 0;
	char * data_bit = 0;
	char * parity = 0;
	char * stop_bit = 0;
	u8_t c;
	s32_t i;

	if(argc < 2)
	{
		serial_info();
		return 0;
	}

	if( !strcmp((const char *)argv[1],"info") )
	{
		serial_info();
	}
	else if( !strcmp((const char *)argv[1],"send") )
	{
		if(argc != 4)
		{
			printk("usage:\r\n    serial [info]\r\n"
				   "    serial send <DEVICE NAME> <STRING>\r\n"
				   "    serial recv <DEVICE NAME>\r\n"
				   "    serial param <DEVICE NAME> [-b BAUD] [-d DATABITS] [-p PARITY] [-s STOPBITS]\r\n");
			return (-1);
		}
		name = (char *)argv[2];
		str = (char *)argv[3];
		device = search_chrdev_with_type((const char *)name, CHR_DEV_SERIAL);
		if(!device)
		{
			printk(" not found serial device \"%s\"\r\n", name);
			printk(" try 'serial info' for list all of serial devices\r\n");
			return -1;
		}

		if(device->open)
			(device->open)(device);

		if(device->write)
			(device->write)(device, (u8_t *)str, strlen(str));

		if(device->close)
			(device->close)(device);
	}
	else if( !strcmp((const char *)argv[1],"recv") )
	{
		if(argc != 3)
		{
			printk("usage:\r\n    serial [info]\r\n"
				   "    serial send <DEVICE NAME> <STRING>\r\n"
				   "    serial recv <DEVICE NAME>\r\n"
				   "    serial param <DEVICE NAME> [-b BAUD] [-d DATABITS] [-p PARITY] [-s STOPBITS]\r\n");
			return (-1);
		}
		name = (char *)argv[2];
		device = search_chrdev_with_type((const char *)name, CHR_DEV_SERIAL);
		if(!device)
		{
			printk(" not found serial device \"%s\"\r\n", name);
			printk(" try 'serial info' for list all of serial devices\r\n");
			return -1;
		}

		if(device->open)
			(device->open)(device);

		if(device->read)
		{
			read_func = device->read;
			while(1)
			{
				if(read_func(device, &c, 1) == 1)
				{
					if(isprint(c) || isspace(c))
						printk("%c", c);
					else
						printk("<%02x>", c);
				}
				if(ctrlc())
					break;
			}
		}

		if(device->close)
			(device->close)(device);
	}

	else if( !strcmp((const char *)argv[1],"param") )
	{
		if(argc < 3)
		{
			printk("usage:\r\n    serial [info]\r\n"
				   "    serial send <DEVICE NAME> <STRING>\r\n"
				   "    serial recv <DEVICE NAME>\r\n"
				   "    serial param <DEVICE NAME> [-b BAUD] [-d DATABITS] [-p PARITY] [-s STOPBITS]\r\n");
			return (-1);
		}

		for(i=2; i<argc; i++)
		{
			if( !strcmp((const char *)argv[i],"-b") && (argc > i+1))
			{
				baud = (char *)argv[i+1];
				i++;
			}
			else if( !strcmp((const char *)argv[i],"-d") && (argc > i+1))
			{
				data_bit = (char *)argv[i+1];
				i++;
			}
			else if( !strcmp((const char *)argv[i],"-p") && (argc > i+1))
			{
				parity = (char *)argv[i+1];
				i++;
			}
			else if( !strcmp((const char *)argv[i],"-s") && (argc > i+1))
			{
				stop_bit = (char *)argv[i+1];
				i++;
			}
			else if(*argv[i] != '-' && strcmp((const char *)argv[i], "-") != 0)
			{
				name = (char *)argv[i];
				device = search_chrdev_with_type((const char *)name, CHR_DEV_SERIAL);
				if(!device)
				{
					printk(" not found serial device \"%s\"\r\n", name);
					printk(" try 'serial info' for list all of serial devices\r\n");
					return -1;
				}
				if(!(device->ioctl))
				{
					printk(" don't support ioctl function at this device.\r\n");
					return -1;
				}
			}
		}

		if(!name)
		{
			printk("usage:\r\n    serial [info]\r\n"
				   "    serial send <DEVICE NAME> <STRING>\r\n"
				   "    serial recv <DEVICE NAME>\r\n"
				   "    serial param <DEVICE NAME> [-b BAUD] [-d DATABITS] [-p PARITY] [-s STOPBITS]\r\n");
			return (-1);
		}

		if(baud)
		{
			if(!strcmp(baud, "50"))
				param.baud_rate = B50;
			else if(!strcmp(baud, "75"))
				param.baud_rate = B75;
			else if(!strcmp(baud, "110"))
				param.baud_rate = B110;
			else if(!strcmp(baud, "134"))
				param.baud_rate = B134;
			else if(!strcmp(baud, "200"))
				param.baud_rate = B200;
			else if(!strcmp(baud, "300"))
				param.baud_rate = B300;
			else if(!strcmp(baud, "600"))
				param.baud_rate = B600;
			else if(!strcmp(baud, "1200"))
				param.baud_rate = B1200;
			else if(!strcmp(baud, "1800"))
				param.baud_rate = B1800;
			else if(!strcmp(baud, "2400"))
				param.baud_rate = B2400;
			else if(!strcmp(baud, "4800"))
				param.baud_rate = B4800;
			else if(!strcmp(baud, "9600"))
				param.baud_rate = B9600;
			else if(!strcmp(baud, "19200"))
				param.baud_rate = B19200;
			else if(!strcmp(baud, "38400"))
				param.baud_rate = B38400;
			else if(!strcmp(baud, "57600"))
				param.baud_rate = B57600;
			else if(!strcmp(baud, "76800"))
				param.baud_rate = B76800;
			else if(!strcmp(baud, "115200"))
				param.baud_rate = B115200;
			else if(!strcmp(baud, "230400"))
				param.baud_rate = B230400;
			else if(!strcmp(baud, "380400"))
				param.baud_rate = B380400;
			else if(!strcmp(baud, "460800"))
				param.baud_rate = B460800;
			else if(!strcmp(baud, "921600"))
				param.baud_rate = B921600;
			else
			{
				printk("unrecognize the parameter of baud rate \"%s\"\r\n", baud);
				return -1;
			}

			if(device->ioctl(device, IOCTL_WR_SERIAL_BAUD_RATE, (void *)(&(param.baud_rate))) < 0)
			{
				printk("setting serial device's baud rate fail. (%s)\r\n", device->name);
				return -1;
			}
		}

		if(data_bit)
		{
			if(!strcmp(data_bit, "5"))
				param.data_bit = DATA_BITS_5;
			else if(!strcmp(data_bit, "6"))
				param.data_bit = DATA_BITS_6;
			else if(!strcmp(data_bit, "7"))
				param.data_bit = DATA_BITS_7;
			else if(!strcmp(data_bit, "8"))
				param.data_bit = DATA_BITS_8;
			else
			{
				printk("unrecognize the parameter of data bits \"%s\"\r\n", data_bit);
				return -1;
			}

			if(device->ioctl(device, IOCTL_WR_SERIAL_DATA_BITS, (void *)(&(param.data_bit))) < 0)
			{
				printk("setting serial device's data bits fail. (%s)\r\n", device->name);
				return -1;
			}
		}

		if(parity)
		{
			if(!strcmp(parity, "N"))
				param.parity = PARITY_NONE;
			else if(!strcmp(parity, "E"))
				param.parity = PARITY_EVEN;
			else if(!strcmp(parity, "O"))
				param.parity = PARITY_ODD;
			else
			{
				printk("unrecognize the parameter of parity \"%s\"\r\n", parity);
				return -1;
			}

			if(device->ioctl(device, IOCTL_WR_SERIAL_PARITY_BIT, (void *)(&(param.parity))) < 0)
			{
				printk("setting serial device's parity fail. (%s)\r\n", device->name);
				return -1;
			}
		}

		if(stop_bit)
		{
			if(!strcmp(stop_bit, "1"))
				param.stop_bit = STOP_BITS_1;
			else if(!strcmp(stop_bit, "1.5"))
				param.stop_bit = STOP_BITS_1_5;
			else if(!strcmp(stop_bit, "2"))
				param.stop_bit = STOP_BITS_2;
			else
			{
				printk("unrecognize the parameter of stop bits \"%s\"\r\n", stop_bit);
				return -1;
			}

			if(device->ioctl(device, IOCTL_WR_SERIAL_STOP_BITS, (void *)(&(param.stop_bit))) < 0)
			{
				printk("setting serial device's stop bit fail. (%s)\r\n", device->name);
				return -1;
			}
		}

		printk("setting serial device's parameter successed. (%s)\r\n", device->name);
		return 0;
	}
	else
	{
		printk("serial: invalid option '%s'\r\n", argv[1]);
		printk("usage:\r\n    serial [info]\r\n"
			   "    serial send <DEVICE NAME> <STRING>\r\n"
			   "    serial recv <DEVICE NAME>\r\n"
			   "    serial param <DEVICE NAME> [-b BAUD] [-d DATABITS] [-p PARITY] [-s STOPBITS]\r\n");
		printk("try 'help serial' for more information.\r\n");
		return (-1);
	}

	return 0;
}

static struct command serial_cmd = {
	.name		= "serial",
	.func		= serial,
	.desc		= "serial device manager\r\n",
	.usage		= "\r\n    serial [info]\r\n"
				  "    serial send <DEVICE NAME> <STRING>\r\n"
				  "    serial recv <DEVICE NAME>\r\n"
				  "    serial param <DEVICE NAME> [-b BAUD] [-d DATABITS] [-p PARITY] [-s STOPBITS]\r\n",
	.help		= "    manage all of serial device\r\n"
				  "    info    for list all of serial device.\r\n"
				  "    send    send a STRING via DEVICE NAME(the serial name).\r\n"
				  "    recv    receice character from DEVICE NAME(the serial name) and print it.\r\n"
				  "    param   setting the serial parameter.\r\n"
				  "    -b      change DEVICE NAME's baud rate.\r\n"
				  "    -d      change DEVICE NAME's data bits. DATABITS for '5' '6' '7' or '8'\r\n"
				  "    -p      change DEVICE NAME's parity bit, PARITY for 'N' or 'E' or 'O'.\r\n"
				  "    -s      change DEVICE NAME's stop bits, STOPBITS for '1' or '1.5' or '2'.\r\n"
};

static __init void serial_cmd_init(void)
{
	if(!command_register(&serial_cmd))
		LOG_E("register 'serial' command fail");
}

static __exit void serial_cmd_exit(void)
{
	if(!command_unregister(&serial_cmd))
		LOG_E("unregister 'serial' command fail");
}

command_initcall(serial_cmd_init);
command_exitcall(serial_cmd_exit);

#endif
