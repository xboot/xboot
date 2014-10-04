/*
 * init/mode/run_shell.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <shell/exec.h>
#include <shell/readline.h>

void run_shell_mode(void)
{
	char * p;
	char cwd[256];
	char prompt[256];

	do {
		getcwd(cwd, sizeof(cwd));
		sprintf(prompt, "xboot: %s$ ", cwd);

		p = readline(prompt);
		exec_cmdline(p);
		free(p);
	} while(xboot_get_mode() == MODE_SHELL);
}
