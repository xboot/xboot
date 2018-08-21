/*
 * kernel/command/cmd-fileram.c
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

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    fileram -f <file> <addr>\r\n");
	printf("    fileram -r <addr> <size> <file>\r\n");
}

static int do_fileram(int argc, char ** argv)
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
			usage();
			return -1;
		}

		filename = (char *)argv[2];
		addr = strtoul((const char *)argv[3], NULL, 0);
		size = 0;

		fd = open(filename, O_RDONLY, (S_IRUSR|S_IRGRP|S_IROTH));
		if(fd < 0)
		{
			printf("can not to open the file '%s'\r\n", filename);
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
		printf("copy file %s to ram 0x%08lx ~ 0x%08lx.\r\n", filename, addr, addr + size);
	}
	else if( !strcmp((const char *)argv[1], "-r") )
	{
		if(argc != 5)
		{
			usage();
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
			printf("failed to write file from ram\r\n");
			return -1;
		}
		close(fd);
		printf("copy ram 0x%08lx ~ 0x%08lx to file %s.\r\n", addr, addr + size, filename);
	}
	else
	{
		usage();
		return (-1);
	}

	return 0;
}

static struct command_t cmd_fileram = {
	.name	= "fileram",
	.desc	= "copy file to ram or ram to file",
	.usage	= usage,
	.exec	= do_fileram,
};

static __init void fileram_cmd_init(void)
{
	register_command(&cmd_fileram);
}

static __exit void fileram_cmd_exit(void)
{
	unregister_command(&cmd_fileram);
}

command_initcall(fileram_cmd_init);
command_exitcall(fileram_cmd_exit);
