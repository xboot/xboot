/*
 * kernel/command/cmd_env.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jjjstudio@gmail.com>
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <shell/env.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_ENV) && (CONFIG_COMMAND_ENV > 0)

extern struct hlist_head env_hash[CONFIG_ENV_HASH_SIZE];

/*
 * print environment variable.
 */
static void print_env(void)
{
	struct env_list * list;
	struct env_list ** env_list_array;
	x_s32 env_list_num = 0;
	struct hlist_node * pos;
	x_s32 i, j, swaps;

	for(i = 0, env_list_num = 0; i < CONFIG_ENV_HASH_SIZE; i++)
	{
		hlist_for_each_entry(list,  pos, &(env_hash[i]), node)
		{
			env_list_num++;
		}
	}

	if(env_list_num <= 0)
		return;

	env_list_array = malloc(sizeof(struct env_list *) * env_list_num);
	if(!env_list_array)
	{
		printk("malloc env_list_array fail for sort env list\r\n");
		return;
	}

	for(i = 0,j = 0; i < CONFIG_ENV_HASH_SIZE; i++)
	{
		hlist_for_each_entry(list,  pos, &(env_hash[i]), node)
		{
			env_list_array[j++] = list;
		}
	}

	/* sort env list (trivial bubble sort) */
	for (i = env_list_num - 1; i > 0; --i)
	{
		swaps = 0;
		for(j=0; j<i; ++j)
		{
			if (strcmp((x_s8*)env_list_array[j]->env.key, (x_s8*)env_list_array[j + 1]->env.key) > 0)
			{
				list = env_list_array[j];
				env_list_array[j] = env_list_array[j + 1];
				env_list_array[j + 1] = list;
				++swaps;
			}
		}
		if(!swaps)
			break;
	}

	/* display env information */
	for (i=0; i<env_list_num; i++)
	{
		printk(" %s=%s\r\n",env_list_array[i]->env.key, env_list_array[i]->env.value);
	}
	free(env_list_array);
}

static void usage(void)
{
	printk("usage:\r\n    env [-s] [<-a|-d|-m> NAME VALUE]\r\n");
}

static x_s32 env(x_s32 argc, const x_s8 **argv)
{
	x_s32 i;
	x_s8 *p;

	if(argc == 1)
	{
		print_env();
	}
	else if(argc >= 2)
	{
		if(strcmp(argv[1], (x_s8*)"-a") == 0)
		{
			if(argc==4)
				env_add((char *)argv[2], (char *)argv[3]);
			else
				usage();
		}
		else if(strcmp(argv[1], (x_s8*)"-r") == 0)
		{
			for(i=2; i<argc; ++i)
				env_remove((char *)argv[i]);
		}
		else if(strcmp(argv[1], (x_s8*)"-s") == 0)
		{
			if(argc==2)
			{
				if(env_save("/etc/environment.xml"))
				{
					printk("save environment variable successes.\r\n");
					return 0;
				}
				else
				{
					printk("save environment variable fail.\r\n");
					return -1;
				}
			}
			else
				usage();
		}
		else
		{
			for(i=1; i<argc; ++i)
			{
				p = (x_s8*)env_get((char *)argv[i], NULL);
				if(p)
					printk(" %s=%s\r\n",argv[i], p);
				else
					printk(" no environment variable '%s'\r\n", argv[i]);
			}
		}
	}

	return 0;
}

static struct command env_cmd = {
	.name		= "env",
	.func		= env,
	.desc		= "display environment variable\r\n",
	.usage		= "env [-s] [<-a|-r> NAME VALUE]\r\n",
	.help		= "    no arguments for list of all variable. for specific variable "
				  "you can type 'env' with one or more variable as arguments.\r\n"
				  "    -a    append variable to the environment\r\n"
				  "    -r    remove variable from the environment\r\n"
				  "    -s    save environment variable\r\n"
};

static __init void env_cmd_init(void)
{
	if(!command_register(&env_cmd))
		LOG_E("register 'env' command fail");
}

static __exit void env_cmd_exit(void)
{
	if(!command_unregister(&env_cmd))
		LOG_E("unregister 'env' command fail");
}

module_init(env_cmd_init, LEVEL_COMMAND);
module_exit(env_cmd_exit, LEVEL_COMMAND);

#endif
