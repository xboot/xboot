/*
 * kernel/command/cmd_ls.c
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
#include <xboot/log.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <command/command.h>
#include <console/console.h>
#include <fs/fileio.h>


#if	defined(CONFIG_COMMAND_LS) && (CONFIG_COMMAND_LS > 0)

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
	    	printk("d");
	    if(S_ISCHR(st->st_mode))
	    	printk("c");
	    if(S_ISBLK(st->st_mode))
	    	printk("b");
	    if(S_ISREG(st->st_mode))
	    	printk("-");
	    if(S_ISLNK(st->st_mode))
	    	printk("l");
	    if(S_ISFIFO(st->st_mode))
	    	printk("p");
	    if(S_ISSOCK(st->st_mode))
	    	printk("s");

	    printk("%s%s%s", rwx[(st->st_mode & S_IRWXU) >> 16], rwx[(st->st_mode & S_IRWXG) >> 19], rwx[(st->st_mode & S_IRWXO) >> 22]);

		if((st->st_uid == 0) && (st->st_gid == 0))
			printk(" %s %s", "root", "root");
		else
			printk(" %4d %4d", st->st_uid, st->st_gid);

		printk(" %8ld", (s32_t)st->st_size);

		printk(" %s\r\n", name);
	}
	else
	{
		len = strlen(name);
		rlen = ((u32_t)(len + 12) / 12) * 12;

		if(position + rlen >= width)
		{
			printk("\r\n");
			position = 0;
		}

		printk("%s", name);
		for(i=len; i < rlen; i++)
		{
			printk(" ");
		}
		position += rlen;
	}
}

static void do_list(const char * path, u32_t flags, u32_t width)
{
	char buf[MAX_PATH];
	struct stat st;
	struct dirent * entry;
	void * dir;
	s32_t n_file = 0;

	/* initial position for print_entry */
	position = 0;

	if(stat(path, &st) != 0)
	{
		printk("ls: cannot access %s: No such file or directory\r\n", path);
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
			printk("\r\n");
	}
}

static int ls(int argc, char ** argv)
{
	s32_t width, height;
	u32_t flags = 0;
	s32_t c = 0;
	s8_t ** v;
	s32_t i;

	if( (v = malloc(sizeof(s8_t *) * argc)) == NULL)
		return -1;

	if(!console_getwh(get_console_stdout(), &width, &height))
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

static struct command ls_cmd = {
	.name		= "ls",
	.func		= ls,
	.desc		= "list directory contents\r\n",
	.usage		= "ls [-l] [-a] [FILE]...\r\n",
	.help		= "    list information about the FILE\r\n"
				  "    the current directory by default.\r\n"
				  "    -l    use a long listing format\r\n"
				  "    -a    do not ignore entries starting with .\r\n"
};

static __init void ls_cmd_init(void)
{
	if(!command_register(&ls_cmd))
		LOG_E("register 'ls' command fail");
}

static __exit void ls_cmd_exit(void)
{
	if(!command_unregister(&ls_cmd))
		LOG_E("unregister 'ls' command fail");
}

command_initcall(ls_cmd_init);
command_exitcall(ls_cmd_exit);

#endif
