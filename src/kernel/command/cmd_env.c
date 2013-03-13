/*
 * kernel/command/cmd_env.c
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
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <runtime.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_ENV) && (CONFIG_COMMAND_ENV > 0)

static int env(int argc, char ** argv)
{
	struct environ_t * environ = &(runtime_get()->__environ);
	struct environ_t * p;
	bool_t save = FALSE;
	int i;

	for(i=1; i<argc; ++i)
	{
		if(strchr(argv[i], '='))
		{
			putenv(argv[i]);
		}
		else if(strcmp(argv[i], "-s") == 0)
		{
			save = TRUE;
		}
		else
		{
			printk("usage:\r\n    env [-s] [NAME=VALUE] ...\r\n");
			return -1;
		}
	}

	for(p = environ->next; p != environ; p = p->next)
	{
		printk(" %s\n", p->content);
	}

	if(save)
		saveenv("/etc/environment.xml");

	return 0;
}

static struct command env_cmd = {
	.name		= "env",
	.func		= env,
	.desc		= "display environment variable\r\n",
	.usage		= "env [-s] [NAME=VALUE] ...\r\n",
	.help		= "    no arguments for list of all variable.\r\n"
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

command_initcall(env_cmd_init);
command_exitcall(env_cmd_exit);

#endif
