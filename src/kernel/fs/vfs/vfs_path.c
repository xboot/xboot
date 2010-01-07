/*
 * kernel/fs/vfs/vfs_path.c
 *
 * Copyright (c) 2007-2008  jianjun jiang <jjjstudio@gmail.com>
 * website: http://xboot.org
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
#include <malloc.h>
#include <xboot/initcall.h>
#include <fs/fs.h>
#include <fs/vfs/vfs.h>

/* the size of fd size */
#define	FD_SIZE						(32)

/*
 * file descriptor
 */
static struct file * file_desc[FD_SIZE];

/*
 * buffer for storing cwd path
 */
static char cwd[MAX_PATH];

/*
 * current work directory's fp
 */
static struct file * cwdfp;

/*
 * allocate a file descriptor.
 */
x_s32 fd_alloc(x_s32 low)
{
	x_s32 fd;

    if( (low < 0) || (low >= FD_SIZE) )
    	return -1;

    for( fd = low; fd < FD_SIZE; fd++ )
    {
        if( file_desc[fd] == NULL )
        {
            return fd;
        }
    }

    return -1;
}

/*
 * free a file descriptor.
 */
x_s32 fd_free(x_s32 fd)
{
    if( (fd < 0) || (fd >= FD_SIZE) )
    	return -1;

    file_desc[fd] = NULL;

    return 0;
}

/*
 * map a descriptor to a file object.
 */
struct file * get_fp(x_s32 fd)
{
    if( (fd < 0) || (fd >= FD_SIZE) )
    	return NULL;

    return file_desc[fd];
}

/*
 * set a descriptor with a file object.
 */
x_s32 set_fp(x_s32 fd, struct file *fp)
{
    if( (fd < 0) || (fd >= FD_SIZE) )
    	return -1;

    file_desc[fd] = fp;
    return 0;
}

/*
 * convert to full path from the cwd and path.
 */
x_s32 vfs_path_conv(const char * path, char * full)
{
	char *src, *tgt, *p, *end;
	x_size len = 0;

	if(path == NULL)
		return -1;

	len = strlen((const x_s8 *)path);
	if(len >= MAX_PATH)
		return -1;

	if(strlen((const x_s8 *)cwd) + len >= MAX_PATH)
		return -1;

	//FIXME
	char *t = malloc(strlen((const x_s8 *)path)+1);
	strcpy((x_s8 *)t, (const x_s8 *)path);
	src = (char *)t;

	tgt = full;
	end = src + len;

	if(path[0] == '/')
	{
		*tgt++ = *src++;
		len++;
	}
	else
	{
		strlcpy((x_s8 *)full, (const x_s8 *)cwd, MAX_PATH);
		len = strlen((const x_s8 *)cwd);
		tgt += len;
		if(len > 1 && path[0] != '.')
		{
			*tgt = '/';
			tgt++;
			len++;
		}
	}

	while(*src)
	{
		p = src;
		while(*p != '/' && *p != '\0')
			p++;
		*p = '\0';
		if(!strcmp((const x_s8 *)src, (const x_s8 *)".."))
		{
			if(len >= 2)
			{
				len -= 2;
				tgt -= 2;	/* skip previous '/' */
				while(*tgt != '/')
				{
					tgt--;
					len--;
				}
				if(len == 0)
				{
					tgt++;
					len++;
				}
			}
		}
		else if(!strcmp((const x_s8 *)src, (const x_s8 *)"."))
		{
			/* ignore "." */
		}
		else
		{
			while(*src != '\0')
			{
				*tgt++ = *src++;
				len++;
			}
		}
		if(p == end)
			break;
		if(len > 0 && *(tgt - 1) != '/')
		{
			*tgt++ = '/';
			len++;
		}
		src = p + 1;
	}
	*tgt = '\0';

	free(t);
	return 0;
}

/*
 * set current work directory
 */
void vfs_setcwd(const char * path)
{
	if(!path)
		return;

	if(strlen((const x_s8 *)path) < MAX_PATH)
		strcpy((x_s8 *)cwd, (const x_s8 *)path);
}

/*
 * get current work directory
 */
char * vfs_getcwd(char * buf, x_s32 size)
{
    if( size == 0 )
        return NULL;

    if( size < strlen((const x_s8 *)cwd) + 1 )
        return NULL;

    strlcpy((x_s8 *)buf, (const x_s8 *)cwd, size);

    return buf;
}

/*
 * set current work directory's fp
 */
void vfs_setcwdfp(struct file * fp)
{
	cwdfp = fp;
}

/*
 * get current work directory's fp
 */
struct file * vfs_getcwdfp(void)
{
	return cwdfp;
}

/*
 * fd pure init
 */
static __init void fd_pure_sync_init(void)
{
	x_s32 i;

    for( i = 0; i < FD_SIZE; i++ )
    	file_desc[i] = NULL;

    strcpy((x_s8 *)cwd, (const x_s8 *)"/");
    cwdfp = NULL;
}

module_init(fd_pure_sync_init, LEVEL_PURE_SYNC);
