/*
 * kernel/shell/system.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <shell/parser.h>
#include <shell/system.h>
#include <framework/vm.h>
#include <command/command.h>

int system(const char * cmdline)
{
	struct command_t * cmd;
	char **args;
	char *p, *buf, *pos;
	size_t len;
	int n, ret;

	if(!cmdline)
		return 0;

	len = strlen(cmdline);
	buf = malloc(len + 2);
	if(!buf)
		return 0;
	memcpy(buf, cmdline, len);
	memcpy(buf + len, " ", 2);

	p = buf;
	while(*p)
	{
		if(parser(p, &n, &args, &pos))
		{
			if(n > 0)
			{
				if((cmd = search_command(args[0])))
					ret = cmd->exec(n, args);
				else
					ret = vmexec(n, args);
				if((ret < 0) && pos)
				{
			    	printf(" when exec \'%s\' return an error code (%ld).\r\n", args[0], ret);
			    	free(args[0]);
			    	free(args);
			    	break;
				}
    		}
			free(args[0]);
			free(args);
    	}

		if(!pos)
			*p = 0;
		else
			p = pos;
    }

	free(buf);
	return 1;
}
EXPORT_SYMBOL(system);
