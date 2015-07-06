/*
 * kernel/command/cmd-mount.c
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

#include <block/loop.h>
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
	bool_t loop_flag = FALSE;
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
			if(!strcmp((const char *)argv[i+1], "loop"))
				loop_flag = TRUE;
			else if(!strcmp((const char *)argv[i+1], "ro"))
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

	if(loop_flag)
	{
		pdev = dev;
		if(stat(pdev, &st) != 0)
		{
			printf("cannot access %s: no such file\r\n", pdev);
			return -1;
		}

		if(!S_ISREG(st.st_mode))
		{
			printf("it's not a regulation file\r\n", pdev);
			return -1;
		}

		if(!register_loop(pdev))
		{
			printf("register a loop block device fail\r\n");
			return -1;
		}

		blk = search_loop(pdev);
		if(!blk)
		{
			printf("special loop block device not found\r\n");
			return -1;
		}

		dev = (char *)blk->name;
	}

	if(loop_flag)
		mount_flag |= MOUNT_LOOP;

	if(ro_flag)
		mount_flag |= MOUNT_RDONLY;

	if(mount(dev, dir , fstype, (mount_flag & MOUNT_MASK)) != 0)
	{
		if(loop_flag)
			unregister_loop(pdev);

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
