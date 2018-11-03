/*
 * kernel/shell/system.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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
#include <framework/vm.h>
#include <shell/parser.h>
#include <shell/system.h>

int system(const char * cmdline)
{
	struct command_t * cmd;
	char ** args;
	char * p, * buf, * pos;
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
					ret = vmexec(args[0]);
				if((ret < 0) && pos)
				{
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
