/*
 * kernel/command/cmd-nvmem.c
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
#include <nvmem/nvmem.h>
#include <command/command.h>

static void usage(void)
{
	struct device_t * pos, * n;

	printf("usage:\r\n");
	printf("    nvmem <device>                     - Print nvmem all key-value\r\n");
	printf("    nvmem <device> set <key> [<value>] - Set or clear nvmem key-value\r\n");
	printf("    nvmem <device> get <key> [...]     - Get nvmem key-value\r\n");
	printf("    nvmem <device> clear               - Clear nvmem all key-value\r\n");

	printf("supported device list:\r\n");
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_NVMEM], head)
	{
		printf("    %s\r\n", pos->name);
	}
}

static int do_nvmem(int argc, char ** argv)
{
	struct nvmem_t * m;
	struct hmap_entry_t * e;
	int i;

	if(argc < 2)
	{
		usage();
		return -1;
	}
	else
	{
		m = search_nvmem(argv[1]);
		if(m)
		{
			if(argc > 2)
			{
				if(!strcmp(argv[2], "set"))
				{
					if(argc > 4)
						nvmem_set(m, argv[3], argv[4]);
					else if(argc == 4)
						nvmem_set(m, argv[3], NULL);
					else
					{
						usage();
						return -1;
					}
				}
				else if(!strcmp(argv[2], "get"))
				{
					if(argc > 3)
					{
						for(i = 3; i < argc; i++)
							printf("%s = %s\r\n", argv[i], nvmem_get(m, argv[i], NULL));
					}
					else
					{
						usage();
						return -1;
					}
				}
				else if(!strcmp(argv[2], "clear"))
				{
					nvmem_clear(m);
				}
				else
				{
					usage();
					return -1;
				}
			}
			else
			{
				hmap_sort(m->kvdb.map);
				hmap_for_each_entry(e, m->kvdb.map)
				{
					printf("%s = %s\r\n", e->key, e->value);
				}
			}
		}
		else
		{
			usage();
			return -1;
		}
	}
	return 0;
}

static struct command_t cmd_nvmem = {
	.name	= "nvmem",
	.desc	= "nvmem set or get tool",
	.usage	= usage,
	.exec	= do_nvmem,
};

static __init void nvmem_cmd_init(void)
{
	register_command(&cmd_nvmem);
}

static __exit void nvmem_cmd_exit(void)
{
	unregister_command(&cmd_nvmem);
}

command_initcall(nvmem_cmd_init);
command_exitcall(nvmem_cmd_exit);
