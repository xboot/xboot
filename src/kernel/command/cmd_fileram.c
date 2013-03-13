/*
 * kernel/command/cmd_fileram.c
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
#include <malloc.h>
#include <stdlib.h>
#include <time/tick.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>
#include <fs/fileio.h>


#if	defined(CONFIG_COMMAND_FILERAM) && (CONFIG_COMMAND_FILERAM > 0)

static void usage(void)
{
	printk("usage:\r\n");
	printk("    fileram -f <file> <addr>\r\n");
	printk("    fileram -r <addr> <size> <file>\r\n");
}

static int fileram(int argc, char ** argv)
{
	char * filename;
	s32_t fd;
	u32_t addr, size = 0;
	s32_t n;

	if(argc != 4 && argc != 5)
	{
		usage();
		return -1;
	}

	if( !strcmp((const char *)argv[1],"-f") )
	{
		if(argc != 4)
		{
			printk("usage:\r\n");
			printk("    fileram -f <file> <addr>\r\n");
			return -1;
		}

		filename = (char *)argv[2];
		addr = strtoul((const char *)argv[3], NULL, 0);
		size = 0;

		fd = open(filename, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
		if(fd < 0)
		{
			printk("can not to open the file '%s'\r\n", filename);
			return -1;
		}

	    for(;;)
	    {
	        n = read(fd, (void *)(addr + size), SZ_512K);
	        if(n <= 0)
	        	break;
			size += n;
	    }

		close(fd);
		printk("copy file %s to ram 0x%08lx ~ 0x%08lx.\r\n", filename, addr, addr + size);
	}
	else if( !strcmp((const char *)argv[1], "-r") )
	{
		if(argc != 5)
		{
			printk("usage:\r\n");
			printk("    fileram -r <addr> <size> <file>\r\n");
			return (-1);
		}

		addr = strtoul((const char *)argv[2], NULL, 0);
		size = strtoul((const char *)argv[3], NULL, 0);
		filename = (char *)argv[4];

		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH));
		if(fd < 0)
			return -1;

		n = write(fd, (void *)addr, size);
		if( n != size )
		{
			close(fd);
			unlink(filename);
			printk("failed to write file from ram\r\n");
			return -1;
		}
		close(fd);
		printk("copy ram 0x%08lx ~ 0x%08lx to file %s.\r\n", addr, addr + size, filename);
	}
	else
	{
		usage();
		return (-1);
	}

	return 0;
}

static struct command fileram_cmd = {
	.name		= "fileram",
	.func		= fileram,
	.desc		= "copy file to ram or ram to file\r\n",
	.usage		= "fileram [ -f <file> <addr> ] | [ -r <addr> <size> <file> ]\r\n",
	.help		= "    copy file to ram or ram to file\r\n"
				  "    -f      copy file to ram\r\n"
				  "    -r      copy ram to file\r\n"
				  "    addr    memory base address.\r\n"
				  "    size    memory length.\r\n"
				  "    file    the file name will be operation.\r\n"
};

static __init void fileram_cmd_init(void)
{
	if(!command_register(&fileram_cmd))
		LOG_E("register 'fileram' command fail");
}

static __exit void fileram_cmd_exit(void)
{
	if(!command_unregister(&fileram_cmd))
		LOG_E("unregister 'fileram' command fail");
}

command_initcall(fileram_cmd_init);
command_exitcall(fileram_cmd_exit);

#endif
