/*
 * kernel/fs/vfs/vfs_path.c
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
#include <malloc.h>
#include <xboot/initcall.h>
#include <fs/fs.h>
#include <fs/vfs/vfs.h>

/* the size of fd size */
#define	FD_SIZE						(256)

/*
 * file descriptor
 */
static struct file_t * file_desc[FD_SIZE];

/*
 * buffer for storing cwd path
 */
static char cwd[MAX_PATH];

/*
 * current work directory's fp
 */
static struct file_t * cwdfp;

/*
 * allocate a file descriptor.
 */
int fd_alloc(int low)
{
	int fd;

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
int fd_free(int fd)
{
    if( (fd < 0) || (fd >= FD_SIZE) )
    	return -1;

    file_desc[fd] = NULL;

    return 0;
}

/*
 * map a descriptor to a file object.
 */
struct file_t * get_fp(int fd)
{
    if( (fd < 0) || (fd >= FD_SIZE) )
    	return NULL;

    return file_desc[fd];
}

/*
 * set a descriptor with a file object.
 */
int set_fp(int fd, struct file_t *fp)
{
    if( (fd < 0) || (fd >= FD_SIZE) )
    	return -1;

    file_desc[fd] = fp;
    return 0;
}

/*
 * convert to full path from the cwd and path by removing all "." and ".."
 */
int vfs_path_conv(const char * path, char * full)
{
	char *p, *q, *s;
	int left_len, full_len;
	char left[MAX_PATH], next_token[MAX_PATH];

    if(path[0] == '/')
    {
    	full[0] = '/';
		full[1] = '\0';
		if(path[1] == '\0')
			return 0;

		full_len = 1;
		left_len = strlcpy(left, (const char *)(path + 1), sizeof(left));
	}
    else
    {
		strlcpy(full, cwd, MAX_PATH);
		full_len = strlen(full);
		left_len = strlcpy(left, path, sizeof(left));
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
		if((int)(s - left) >= sizeof(next_token))
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
		if(next_token[0] == '\0' || strcmp(next_token, ".") == 0)
		{
			continue;
		}
		else if(strcmp(next_token, "..") == 0)
		{
			/*
			 * strip the last path component except when we have single '/'
			 */
			if(full_len > 1)
			{
				full[full_len - 1] = '\0';
				q = strrchr(full, '/') + 1;
				*q = '\0';
				full_len = q - full;
			}
			continue;
		}

		full_len = strlcat(full, next_token, MAX_PATH);
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

	if(strlen(path) < MAX_PATH)
		strcpy(cwd, path);
}

/*
 * get current work directory
 */
char * vfs_getcwd(char * buf, size_t size)
{
    if( size == 0 )
        return NULL;

    if( size < strlen(cwd) + 1 )
        return NULL;

    strlcpy(buf, cwd, size);

    return buf;
}

/*
 * set current work directory's fp
 */
void vfs_setcwdfp(struct file_t * fp)
{
	cwdfp = fp;
}

/*
 * get current work directory's fp
 */
struct file_t * vfs_getcwdfp(void)
{
	return cwdfp;
}

void vfs_fd_init(void)
{
	int i;

    for( i = 0; i < FD_SIZE; i++ )
    	file_desc[i] = NULL;

    strcpy(cwd, "/");
    cwdfp = NULL;
}
