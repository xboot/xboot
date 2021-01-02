/*
 * kernel/command/cmd-setting.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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
	printf("    setting                     - Print all setting\r\n");
	printf("    setting set <key> [<value>] - Set or clear key\r\n");
	printf("    setting get <key>           - Get value of key\r\n");
	printf("    setting clear               - Clear all setting key\r\n");
}

static int do_setting(int argc, char ** argv)
{
	int i;

	if(argc < 2)
	{
		setting_summary();
		return 0;
	}

	if(!strcmp(argv[1], "set"))
	{
		if(argc > 3)
			setting_set(argv[2], argv[3]);
		else if(argc == 3)
			setting_set(argv[2], NULL);
		else
		{
			usage();
			return -1;
		}
	}
	else if(!strcmp(argv[1], "get"))
	{
		if(argc > 2)
		{
			for(i = 2; i < argc; i++)
				printf("%s = %s\r\n", argv[i], setting_get(argv[i], NULL));
		}
		else
		{
			usage();
			return -1;
		}
	}
	else if(!strcmp(argv[1], "clear"))
	{
		setting_clear();
	}
	else
	{
		usage();
		return -1;
	}

	return 0;
}

static struct command_t cmd_setting = {
	.name	= "setting",
	.desc	= "display the global setting",
	.usage	= usage,
	.exec	= do_setting,
};

static __init void setting_cmd_init(void)
{
	register_command(&cmd_setting);
}

static __exit void setting_cmd_exit(void)
{
	unregister_command(&cmd_setting);
}

command_initcall(setting_cmd_init);
command_exitcall(setting_cmd_exit);
