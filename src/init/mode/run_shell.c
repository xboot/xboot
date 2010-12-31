/*
 * int/mode/run_shell.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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
#include <vsprintf.h>
#include <malloc.h>
#include <xboot/printk.h>
#include <fs/fsapi.h>
#include <console/console.h>
#include <shell/exec.h>
#include <shell/env.h>
#include <shell/readline.h>
#include <mode/mode.h>

/*
 * running the shell mode
 */
void run_shell_mode(void)
{
	x_s8 * p;
	x_s8 cwd[256];
	x_s8 prompt[256];

	console_cls(get_stdout());

	do {
		getcwd((char *)cwd, sizeof(cwd));
		sprintf(prompt, (x_s8 *)"%s: %s$ ", (x_s8 *)env_get("prompt", "xboot"), cwd);

		p = readline((const x_s8 *)prompt);
		printk("\r\n");

		exec_cmdline(p);
		free(p);
	} while(xboot_get_mode() == MODE_SHELL);
}
