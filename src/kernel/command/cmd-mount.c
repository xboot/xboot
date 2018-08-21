/*
 * kernel/command/cmd-mount.c
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
	printf("    mount <-t fstype> [-o option] <dev> <dir>\r\n");
}

static int do_mount(int argc, char ** argv)
{
	char * fstype = NULL;
	char * dev = NULL, * dir = NULL;
	bool_t ro_flag = FALSE;
	s32_t mount_flag = 0;
	struct block_t * blk;
	struct stat st;
	char * pdev = NULL;
	s32_t i, index = 0;

	if(argc < 5)
	{
		usage();
		return -1;
	}

	for(i=1; i<argc; i++)
	{
		if( !strcmp((const char *)argv[i], "-t") && (argc > i+1) )
		{
			fstype = (char *)argv[i+1];
			i++;
		}
		else if( !strcmp((const char *)argv[i], "-o") && (argc > i+1) )
		{
			if(!strcmp((const char *)argv[i+1], "ro"))
				ro_flag = TRUE;
			else if(!strcmp((const char *)argv[i+1], "rw"))
				ro_flag = FALSE;
			else
			{
				printf("unrecognized the option '%s'\r\n", argv[i+1]);
				return -1;
			}
			i++;
		}
		else
		{
			if(index == 0)
				dev = (char *)argv[i];
			else if(index == 1)
				dir = (char *)argv[i];
			else if(index >= 2)
			{
				usage();
				return -1;
			}
			index++;
		}
	}

	if(!fstype || !dev || !dir)
	{
		usage();
		return -1;
	}

	if(!filesystem_search(fstype))
	{
		printf("the filesystem %s not found\r\n", fstype);
		return -1;
	}

	if(stat(dir, &st) != 0)
	{
		printf("cannot access %s: no such directory\r\n", dir);
		return -1;
	}

	if(!S_ISDIR(st.st_mode))
	{
		printf("the '%s' does not a directory\r\n", dir);
		return -1;
	}

	if(ro_flag)
		mount_flag |= MOUNT_RDONLY;

	if(mount(dev, dir , fstype, (mount_flag & MOUNT_MASK)) != 0)
	{
		printf("mount '%s' filesystem on special device '%s' fail\r\n", fstype, dev);
		return -1;
	}

	return 0;
}

static struct command_t cmd_mount = {
	.name	= "mount",
	.desc	= "mount a file system",
	.usage	= usage,
	.exec	= do_mount,
};

static __init void mount_cmd_init(void)
{
	register_command(&cmd_mount);
}

static __exit void mount_cmd_exit(void)
{
	unregister_command(&cmd_mount);
}

command_initcall(mount_cmd_init);
command_exitcall(mount_cmd_exit);
