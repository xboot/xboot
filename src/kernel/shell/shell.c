/*
 * kernel/shell/shell.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <framework/vm.h>
#include <command/command.h>
#include <shell/readline.h>
#include <shell/parser.h>
#include <shell/shell.h>

static char shell_cwd[VFS_MAX_PATH] = { '/', '\0', };
static int shell_cwd_fd = -1;
static spinlock_t shell_cwd_lock = SPIN_LOCK_INIT();

int shell_realpath(const char * path, char * fpath)
{
	char * p, * q, * s;
	int left_len, full_len;
	char left[VFS_MAX_PATH];
	char next_token[VFS_MAX_PATH];

    if(path[0] == '/')
    {
    	fpath[0] = '/';
		fpath[1] = '\0';
		if(path[1] == '\0')
			return 0;

		full_len = 1;
		left_len = strlcpy(left, (const char *)(path + 1), sizeof(left));
	}
    else
    {
    	spin_lock(&shell_cwd_lock);
		strlcpy(fpath, shell_cwd, VFS_MAX_PATH);
		spin_unlock(&shell_cwd_lock);
		full_len = strlen(fpath);
		left_len = strlcpy(left, path, sizeof(left));
	}
	if((left_len >= sizeof(left)) || (full_len >= VFS_MAX_PATH))
		return -1;

	while(left_len != 0)
	{
		p = strchr(left, '/');
		s = p ? p : left + left_len;
		if((int)(s - left) >= sizeof(next_token))
			return -1;

		memcpy(next_token, left, s - left);
		next_token[s - left] = '\0';
		left_len -= s - left;
		if(p != NULL)
		{
			memmove(left, s + 1, left_len + 1);
		}

		if(fpath[full_len - 1] != '/')
		{
			if (full_len + 1 >= VFS_MAX_PATH)
				return -1;

			fpath[full_len++] = '/';
			fpath[full_len] = '\0';
		}
		if(next_token[0] == '\0' || strcmp(next_token, ".") == 0)
		{
			continue;
		}
		else if(strcmp(next_token, "..") == 0)
		{
			if(full_len > 1)
			{
				fpath[full_len - 1] = '\0';
				q = strrchr(fpath, '/') + 1;
				*q = '\0';
				full_len = q - fpath;
			}
			continue;
		}

		full_len = strlcat(fpath, next_token, VFS_MAX_PATH);
		if(full_len >= VFS_MAX_PATH)
			return -1;
	}

	if(full_len > 1 && fpath[full_len - 1] == '/')
	{
		fpath[full_len - 1] = '\0';
	}
	return 0;
}

const char * shell_getcwd(void)
{
	return &shell_cwd[0];
}

int shell_setcwd(const char * path)
{
	char fpath[VFS_MAX_PATH];
	int fd;

	if(shell_realpath(path, fpath) < 0)
		return -1;

	spin_lock(&shell_cwd_lock);
	fd = vfs_opendir(fpath);
	if(fd >= 0)
	{
		if(shell_cwd_fd >= 0)
			vfs_closedir(shell_cwd_fd);
		shell_cwd_fd = fd;
		if(strlen(fpath) < VFS_MAX_PATH)
			strncpy(shell_cwd, fpath, sizeof(shell_cwd));
		spin_unlock(&shell_cwd_lock);
		return 0;
	}
	spin_unlock(&shell_cwd_lock);

	return -1;
}

int shell_system(const char * cmdline)
{
	struct command_t * cmd;
	char fpath[VFS_MAX_PATH];
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
				{
					ret = cmd->exec(n, args);
				}
				else
				{
					if(shell_realpath(args[0], fpath) < 0)
						ret = -1;
					else
						ret = vmexec(fpath, (n > 1) ? args[1] : NULL);
				}
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

void shell_task(struct task_t * task, void * data)
{
	char prompt[VFS_MAX_PATH];
	char * p;

	while(1)
	{
		sprintf(prompt, "xboot: %s# ", shell_cwd);
		p = readline(prompt);
		shell_system(p);
		free(p);
	}
}
