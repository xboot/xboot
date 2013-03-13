/*
 * kernel/command/cmd_nand.c
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
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <mtd/nand/nfc.h>
#include <mtd/nand/nand.h>
#include <command/command.h>
#include <fs/fileio.h>

#if	defined(CONFIG_COMMAND_NAND) && (CONFIG_COMMAND_NAND > 0)

extern struct nand_list * nand_list;

static void usage(void)
{
	printk("usage:\r\n    nand [list]\r\n"
		   "    nand probe\r\n"
		   "    nand read <device> <offset> <size> <-r addr | -f file>\r\n"
		   "    nand write <device> <offset> <size> <file>\r\n");
}

static void list_nand_device(void)
{
	struct nand_list * list;
	struct list_head * pos;

	for(pos = (&nand_list->entry)->next; pos != (&nand_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct nand_list, entry);
		printk(" \"%s\" - %s (%s)\r\n", list->nand->name, list->nand->info->name, list->nand->manufacturer->name);
	}
}

static int nand(int argc, char ** argv)
{
	struct nand_device * nand;
	u32_t off = 0;
	u32_t size = 0;
	u32_t addr = 0;
	u32_t o, l, len = 0;
	char * filename;
	u8_t * buf;
	s32_t fd;
	s32_t n;

	if(argc < 2)
	{
		list_nand_device();

		return 0;
	}

	if( !strcmp((const char *)argv[1], "list"))
	{
		list_nand_device();

		return 0;
	}
	else if( !strcmp((const char *)argv[1], "probe"))
	{
		nand_flash_probe();
		list_nand_device();

		return 0;
	}
	else if( !strcmp((const char *)argv[1], "read") )
	{
		if(argc != 7)
		{
			usage();
			return -1;
		}

		nand = search_nand_device((const char *)argv[2]);
		if(!nand)
		{
			printk(" not found nand device \"%s\"\r\n", argv[2]);
			printk(" try 'nand list' for list all of nand devices\r\n");
			return -1;
		}

		off = strtoul((const char *)argv[3], NULL, 0);
		size = strtoul((const char *)argv[4], NULL, 0);

		if( !strcmp((const char *)argv[5], "-r") )
		{
			addr = strtoul((const char *)argv[6], NULL, 0);

			if(nand_read(nand, (u8_t *)addr, off, size) != 0)
				return -1;

			printk("read %s 0x%08lx ~ 0x%08lx to ram 0x%08lx.\r\n", nand->name, off, off + size, addr);
		}
		else if( !strcmp((const char *)argv[5], "-f") )
		{
			filename = (char *)argv[6];

			buf = malloc(SZ_64K);
			if(!buf)
				return -1;

			fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
			if(fd < 0)
			{
				free(buf);
				return -1;
			}

			o = off;
			while(len < size)
			{
				if(len + SZ_64K > size)
					l = size - len;
				else
					l = SZ_64K;

				if(nand_read(nand, buf, o, l) != 0)
				{
					close(fd);
					unlink(filename);
					free(buf);
					return -1;
				}

				n = write(fd, (void *)buf, l);
				if( n != l )
				{
					close(fd);
					unlink(filename);
					free(buf);
					return -1;
				}

				o += l;
				len += l;
			}

			close(fd);
			free(buf);

			printk("read %s 0x%08lx ~ 0x%08lx to file %s.\r\n", nand->name, off, off + size, filename);
		}
		else
		{
			usage();
			return -1;
		}
	}
	else if( !strcmp((const char *)argv[1], "write") )
	{

	}
	else if( !strcmp((const char *)argv[1], "erase") )
	{

	}
	else if( !strcmp((const char *)argv[1], "bad") )
	{

	}
	else
	{
		usage();
		return -1;
	}

	return 0;
}

static struct command nand_cmd = {
	.name		= "nand",
	.func		= nand,
	.desc		= "nand flash tool\r\n",
	.usage		= "nand\r\n",
	.help		= "    nand chip manager tool\r\n"
};

static __init void nand_cmd_init(void)
{
	if(!command_register(&nand_cmd))
		LOG_E("register 'nand' command fail");
}

static __exit void nand_cmd_exit(void)
{
	if(!command_unregister(&nand_cmd))
		LOG_E("unregister 'nand' command fail");
}

command_initcall(nand_cmd_init);
command_exitcall(nand_cmd_exit);

#endif
