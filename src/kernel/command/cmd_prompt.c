/*
 * kernel/command/cmd_prompt.c
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
#include <string.h>
#include <malloc.h>
#include <shell/env.h>
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>


#if	defined(CONFIG_COMMAND_PROMPT) && (CONFIG_COMMAND_PROMPT > 0)

static struct command prompt_cmd;

static s32_t prompt(s32_t argc, const s8_t **argv)
{
	switch(argc)
	{
	case 1:
		printk(" the current prompt is \"%s\"\r\n", env_get("prompt", "xboot"));
		break;

	case 2:
		if(strlen((char *)argv[1]) > 32)
		{
			printk(" your argument \"%s\" is too long!\r\n", argv[1]);
			break;
		}

		env_add("prompt", (char*)argv[1]);
		printk(" shell prompt changed to \"%s\".\r\n", env_get("prompt", "xboot"));
		break;

	default:
		printk(" invalid 'prompt' command: too many arguments!\r\n");
		printk(" usage: %s\r\n", prompt_cmd.usage);
		break;
	}
	return 0;
}

static struct command prompt_cmd = {
	.name		= "prompt",
	.func		= prompt,
	.desc		= "change the shell prompt\r\n",
	.usage		= "prompt [STRING]\r\n",
	.help		= "    prompt show or modify the current shell prompt.\r\n"
				  "    no arguments for showing the current prompt.\r\n"
};

static __init void prompt_cmd_init(void)
{
	if(!command_register(&prompt_cmd))
		LOG_E("register 'prompt' command fail");
}

static __exit void prompt_cmd_exit(void)
{
	if(!command_unregister(&prompt_cmd))
		LOG_E("unregister 'prompt' command fail");
}

module_init(prompt_cmd_init, LEVEL_COMMAND);
module_exit(prompt_cmd_exit, LEVEL_COMMAND);

#endif
