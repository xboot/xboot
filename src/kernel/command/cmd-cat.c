/*
 * kernel/command/cmd-cat.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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

static void usage(void)
{
	printf("Usage:\r\n");
	printf("    cat <file> ...\r\n");
}

static int cat_file(const char * filename)
{
    struct stat st;
	char buf[BUFSIZ];
    ssize_t i, n;
    int fd;

	if(stat(filename, &st) != 0)
	{
		printf("cat: %s: No such file or directory\r\n", filename);
		return -1;
	}

	if(S_ISDIR(st.st_mode))
	{
		printf("cat: %s: Is a directory\r\n", filename);
		return -1;
	}

	fd = open(filename, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
	if(fd < 0)
	{
		printf("cat: %s: Can not open\r\n", filename);
		return -1;
	}

	while((n = read(fd, buf, sizeof(buf))) > 0)
	{
		for(i = 0; i < n; i++)
			putchar(buf[i]);
	}
	printf("\r\n");

	close(fd);
	return 0;
}

static int do_cat(int argc, char ** argv)
{
	int i;

	if(argc == 1)
	{
		usage();
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
		if(cat_file(argv[i]) != 0)
			return -1;
	}
	return 0;
}

static struct command_t cmd_cat = {
	.name	= "cat",
	.desc	= "show the contents of a file",
	.usage	= usage,
	.exec	= do_cat,
};

static __init void cat_cmd_init(void)
{
	command_register(&cmd_cat);
}

static __exit void cat_cmd_exit(void)
{
	command_unregister(&cmd_cat);
}

command_initcall(cat_cmd_init);
command_exitcall(cat_cmd_exit);
