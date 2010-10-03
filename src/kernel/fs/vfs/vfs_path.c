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
#define	FD_SIZE						(256)

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

    if(low < 3)
    	low = 3;

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
 * convert to full path from the cwd and path by removing all "." and ".."
 */
x_s32 vfs_path_conv(const char * path, char * full)
{
	x_s8 *p, *q, *s;
	x_s32 left_len, full_len;
	x_s8 left[MAX_PATH], next_token[MAX_PATH];

    if(path[0] == '/')
    {
    	full[0] = '/';
		full[1] = '\0';
		if(path[1] == '\0')
			return 0;

		full_len = 1;
		left_len = strlcpy(left, (const x_s8 *)(path + 1), sizeof(left));
	}
    else
    {
		strlcpy((x_s8 *)full, (const x_s8 *)cwd, MAX_PATH);
		full_len = strlen((const x_s8 *)full);
		left_len = strlcpy(left, (const x_s8 *)path, sizeof(left));
	}
	if((left_len >= sizeof(left)) || (full_len >= MAX_PATH))
		return -1;

	/*
	 * iterate over path components in `left'.
	 */
	while(left_len != 0)
	{
		/*
		 * extract the next path component and adjust left and its length.
		 */
		p = strchr(left, '/');
		s = p ? p : left + left_len;
		if((x_s32)(s - left) >= sizeof(next_token))
			return -1;

		memcpy(next_token, left, s - left);
		next_token[s - left] = '\0';
		left_len -= s - left;
		if(p != NULL)
		{
			memmove(left, s + 1, left_len + 1);
		}

		if(full[full_len - 1] != '/')
		{
			if (full_len + 1 >= MAX_PATH)
				return -1;

			full[full_len++] = '/';
			full[full_len] = '\0';
		}
		if(next_token[0] == '\0' || strcmp(next_token, (const x_s8 *)".") == 0)
		{
			continue;
		}
		else if(strcmp(next_token, (const x_s8 *)"..") == 0)
		{
			/*
			 * strip the last path component except when we have single '/'
			 */
			if(full_len > 1)
			{
				full[full_len - 1] = '\0';
				q = strrchr((const x_s8 *)full, '/') + 1;
				*q = '\0';
				full_len = q - (x_s8 *)full;
			}
			continue;
		}

		full_len = strlcat((x_s8 *)full, next_token, MAX_PATH);
		if(full_len >= MAX_PATH)
			return -1;
	}

	/*
	 * remove trailing slash except when the full pathname is a single '/'
	 */
	if(full_len > 1 && full[full_len - 1] == '/')
	{
		full[full_len - 1] = '\0';
	}

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
