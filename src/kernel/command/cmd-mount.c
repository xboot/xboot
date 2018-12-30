/*
 * kernel/command/cmd-mount.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    mount <-t fstype> [-o option] <dev> <dir>\r\n");
}

static int do_mount(int argc, char ** argv)
{
	char fpath[VFS_MAX_PATH];
	char * fstype = NULL;
	char * dev = NULL;
	char * dir = NULL;
	u32_t mflag;
	int ro = 0;
	int i, index = 0;
	int fd;

	if(argc < 5)
	{
		usage();
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-t") && (argc > i + 1))
		{
			fstype = argv[i + 1];
			i++;
		}
		else if(!strcmp(argv[i], "-o") && (argc > i + 1))
		{
			if(!strcmp(argv[i+1], "ro"))
				ro = 1;
			else if(!strcmp(argv[i + 1], "rw"))
				ro = 0;
			else
			{
				printf("Unrecognized option '%s'\r\n", argv[i + 1]);
				return -1;
			}
			i++;
		}
		else
		{
			if(index == 0)
				dev = argv[i];
			else if(index == 1)
				dir = argv[i];
			else if(index >= 2)
			{
				usage();
				return -1;
			}
			index++;
		}
	}

	if(!fstype || !dir)
	{
		usage();
		return -1;
	}

	if(!search_filesystem(fstype))
	{
		printf("Not found filesystem '%s'\r\n", fstype);
		return -1;
	}

	if(shell_realpath(dir, fpath) < 0)
	{
		printf("Can not convert '%s' to realpath\r\n", dir);
		return -1;
	}

	if(strcmp(fpath, "/") != 0)
	{
		fd = vfs_opendir(fpath);
		if(fd < 0)
		{
			printf("Not found directory '%s'\r\n", fpath);
			return -1;
		}
		else
		{
			vfs_closedir(fd);
		}
	}

	if(ro)
		mflag = MOUNT_RDONLY;
	else
		mflag = MOUNT_RW;

	if(vfs_mount(dev, fpath, fstype, (mflag & MOUNT_MASK)) != 0)
	{
		printf("Fail to mount '%s' filesystem on special device '%s'\r\n", fstype, dev ? dev : "none");
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
