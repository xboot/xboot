/*
 * kernel/command/cmd-ls.c
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

#include <command/command.h>

/* list flags */
#define LSFLAG_DOT				(0x01)		/* list files beginning with "." */
#define LSFLAG_LONG				(0x02)		/* long format */

static s32_t position = 0;
static const char rwx[8][4] = { "---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx" };

static void print_entry(const char * name, struct stat * st, u32_t flags, u32_t width)
{
	u32_t len, rlen;
	u32_t i;

	if(name[0] == '.')
	{
		if((flags & LSFLAG_DOT) == 0)
			return;
	}

	if(flags & LSFLAG_LONG)
	{
	    if(S_ISDIR(st->st_mode))
	    	printf("d");
	    if(S_ISCHR(st->st_mode))
	    	printf("c");
	    if(S_ISBLK(st->st_mode))
	    	printf("b");
	    if(S_ISREG(st->st_mode))
	    	printf("-");
	    if(S_ISLNK(st->st_mode))
	    	printf("l");
	    if(S_ISFIFO(st->st_mode))
	    	printf("p");
	    if(S_ISSOCK(st->st_mode))
	    	printf("s");

	    printf("%s%s%s", rwx[(st->st_mode & S_IRWXU) >> 16], rwx[(st->st_mode & S_IRWXG) >> 19], rwx[(st->st_mode & S_IRWXO) >> 22]);

		if((st->st_uid == 0) && (st->st_gid == 0))
			printf(" %s %s", "root", "root");
		else
			printf(" %4d %4d", st->st_uid, st->st_gid);

		printf(" %8ld", (s32_t)st->st_size);

		printf(" %s\r\n", name);
	}
	else
	{
		len = strlen(name);
		rlen = ((u32_t)(len + 12) / 12) * 12;

		if(position + rlen >= width)
		{
			printf("\r\n");
			position = 0;
		}

		printf("%s", name);
		for(i=len; i < rlen; i++)
		{
			printf(" ");
		}
		position += rlen;
	}
}

static void do_list(const char * path, u32_t flags, u32_t width)
{
	char buf[MAX_PATH];
	struct stat st;
	struct dirent_t * entry;
	void * dir;
	s32_t n_file = 0;

	/* initial position for print_entry */
	position = 0;

	if(stat(path, &st) != 0)
	{
		printf("ls: cannot access %s: No such file or directory\r\n", path);
		return;
	}

	if(S_ISDIR(st.st_mode))
	{
		if( (dir = opendir(path)) == NULL)
	    	return;
		for(;;)
		{
			if( (entry = readdir(dir)) == NULL)
				break;

			buf[0] = 0;
			strlcpy(buf, path, sizeof(buf));
			buf[sizeof(buf) - 1] = '\0';

			if(!strcmp(entry->d_name, "."))
			{
			}
			else if(!strcmp(entry->d_name, ".."))
			{
			}
			else
			{
				strlcat(buf, "/", sizeof(buf));
				strlcat(buf, entry->d_name, sizeof(buf));
			}

			if(stat((const char *)buf, &st) != 0)
				break;

			if( (entry->d_name[0] != '.') || (flags & LSFLAG_DOT) )
			{
				n_file++;
			}

			print_entry((const char *)entry->d_name, &st, flags, width);
		}
		closedir(dir);
	}
	else
	{
		print_entry((const char *)path, &st, flags, width);
		n_file = 1;
	}

	/* auto add \r\n for not long mode */
	if(!(flags & LSFLAG_LONG))
	{
		if( ! ( !(flags & LSFLAG_DOT) && (n_file == 0) ) )
			printf("\r\n");
	}
}

static void usage(void)
{
	printf("usage:\r\n");
	printf("    ls [-l] [-a] [FILE]...\r\n");
}

static int do_ls(int argc, char ** argv)
{
	s32_t width = 80;
	u32_t flags = 0;
	s32_t c = 0;
	s8_t ** v;
	s32_t i;

	if( (v = malloc(sizeof(s8_t *) * argc)) == NULL)
		return -1;

	for(i=1; i<argc; i++)
	{
		if( !strcmp((const char *)argv[i],"-l") )
			flags |= LSFLAG_LONG;
		else if( !strcmp((const char *)argv[i],"-a") )
			flags |= LSFLAG_DOT;
		else
			v[c++] = (s8_t *)argv[i];
	}

	if(c == 0)
		v[c++] = (s8_t *)".";

	for(i=0; i<c; i++)
		do_list((const char *)v[i], flags, width);

	free(v);
	return 0;
}

static struct command_t cmd_ls = {
	.name	= "ls",
	.desc	= "list directory contents",
	.usage	= usage,
	.exec	= do_ls,
};

static __init void ls_cmd_init(void)
{
	register_command(&cmd_ls);
}

static __exit void ls_cmd_exit(void)
{
	unregister_command(&cmd_ls);
}

command_initcall(ls_cmd_init);
command_exitcall(ls_cmd_exit);
