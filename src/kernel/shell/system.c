/*
 * kernel/shell/system.c
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

#include <xboot.h>
#include <shell/parser.h>
#include <shell/system.h>
#include <command/command.h>

void system(const char * cmdline)
{
	struct command_t * cmd;
	char **args;
	char *p, *buf, *pos;
	size_t len;
	int n, ret;

	if(!cmdline)
		return;

	len = strlen(cmdline);
	buf = malloc(len + 2);
	if(!buf)
		return;
	memcpy(buf, cmdline, len);
	memcpy(buf + len, " ", 2);

	p = buf;
	while(*p)
	{
		if(parser(p, &n, &args, &pos))
		{
			if(n > 0)
			{
				cmd = command_search(args[0]);
				if(cmd)
				{
    				ret = cmd->exec(n, args);
    				if(ret != 0)
    				{
    					/*
    					 * if having other command which waitting be exec, abort.
    					 */
    			    	if(pos)
    			    	{
    			    		printf(" when exec \'%s\' command return an error code (%ld).\r\n", args[0], ret);
    			    		free(args[0]);
    			    		free(args);
    			    		break;
    			    	}
    				}
    			}
    			else
    			{
    				printf(" could not found \'%s\' command \r\n", args[0]);
    				printf(" if you want to kown available commands, type 'help'.\r\n");

					/*
					 * if having other command which waitting be exec, abort.
					 */
			    	if(pos)
			    	{
			    		free(args[0]);
			    		free(args);
			    		break;
			    	}
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
}
EXPORT_SYMBOL(system);
