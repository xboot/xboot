/*
 * kernel/command/cmd_cat.c
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
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include <fs/fileio.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>

#if	defined(CONFIG_COMMAND_CAT) && (CONFIG_COMMAND_CAT > 0)

static int cat(int argc, char ** argv)
{
	struct stat st;
	s8_t * buf;
	s32_t fd;
	s32_t i, n;
	s8_t c;

	if(argc != 2)
	{
		printk("usage:\r\n    cat <file>\r\n");
		return -1;
	}

	if(stat((const char *)argv[1], &st) != 0)
	{
		printk("cat: cannot access %s: No such file or directory\r\n", argv[1]);
		return -1;
	}

	if(S_ISDIR(st.st_mode))
	{
		printk("cat: the file %s is a directory\r\n", argv[1]);
		return -1;
	}

	buf = malloc(SZ_64K);
	if(!buf)
		return -1;

	fd = open((const char *)argv[1], O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
	{
		printk("can not to open the file '%s'\r\n", argv[1]);
		free(buf);
		return -1;
	}

	while((n = read(fd, (void *)buf, SZ_64K)) > 0)
	{
		for(i = 0; i < n; i++)
		{
			c = buf[i];

			if(isprint(c) || isspace(c))
			{
				if(c == '\n')
					printk("\r");
				printk("%c", c);
			}
			else
				printk("<%02x>", c);
		}
	}
	printk("\r\n");

	free(buf);
	close(fd);

	return 0;
}

static struct command cat_cmd = {
	.name		= "cat",
	.func		= cat,
	.desc		= "show the contents of a file\r\n",
	.usage		= "cat <file>\r\n",
	.help		= "    show the contents of a file.\r\n"
};

static __init void cat_cmd_init(void)
{
	if(!command_register(&cat_cmd))
		LOG_E("register 'cat' command fail");
}

static __exit void cat_cmd_exit(void)
{
	if(!command_unregister(&cat_cmd))
		LOG_E("unregister 'cat' command fail");
}

command_initcall(cat_cmd_init);
command_exitcall(cat_cmd_exit);

#endif
