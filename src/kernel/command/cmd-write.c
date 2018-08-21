/*
 * kernel/command/cmd-write.c
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
	printf("    write <string> <file>\r\n");
}

static int do_write(int argc, char ** argv)
{
	int fd;
	ssize_t ret;

	if(argc != 3)
	{
		usage();
		return -1;
	}

	fd = open(argv[2], O_WRONLY | O_CREAT, (S_IRUSR|S_IWUSR));
	if(fd < 0)
		return -1;

	ret = write(fd, argv[1], strlen(argv[1]));
	close(fd);
	return ret < 0 ? -1 : 0;
}

static struct command_t cmd_write = {
	.name	= "write",
	.desc	= "write contents to a file",
	.usage	= usage,
	.exec	= do_write,
};

static __init void write_cmd_init(void)
{
	register_command(&cmd_write);
}

static __exit void write_cmd_exit(void)
{
	unregister_command(&cmd_write);
}

command_initcall(write_cmd_init);
command_exitcall(write_cmd_exit);
