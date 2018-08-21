/*
 * kernel/fs/vfs/vfs_mount.c
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
#include <fs/fs.h>
#include <fs/vfs/vfs.h>

/*
 * list for vfs mount points.
 */
struct list_head mount_list = {
	.next	= &mount_list,
	.prev	= &mount_list,
};

/*
 * compare two path strings. return matched length.
 * @path: target path.
 * @root: vfs root path as mount point.
 */
static s32_t count_match(char * path, char * mount_root)
{
	s32_t len = 0;

	while(*path && *mount_root)
	{
		if((*path++) != (*mount_root++))
			break;
		len++;
	}

	if(*mount_root != '\0')
		return 0;

	if((len == 1) && (*(path - 1) == '/'))
		return 1;

	if((*path == '\0') || (*path == '/'))
		return len;

	return 0;
}

/*
 * mark a mount point as busy.
 */
void vfs_busy(struct mount_t * m)
{
	m->m_count++;
}

/*
 * mark a mount point as unbusy.
 */
void vfs_unbusy(struct mount_t * m)
{
	m->m_count--;
}

/*
 * get the root directory and mount point for specified path.
 * @path: full path.
 * @mp: mount point to return.
 * @root: pointer to root directory in path.
 */
s32_t vfs_findroot(char * path, struct mount_t ** mp, char ** root)
{
	struct list_head * pos;
	struct mount_t *m, *tmp;
	s32_t len, max_len = 0;

	if(!path)
		return -1;

	/* find mount point from nearest path */
	m = NULL;

	list_for_each(pos, &mount_list)
	{
		tmp = list_entry(pos, struct mount_t, m_link);
		len = count_match(path, tmp->m_path);
		if(len > max_len)
		{
			max_len = len;
			m = tmp;
		}
	}

	if(m == NULL)
		return -1;

	*root = (char *)(path + max_len);
	if(**root == '/')
		(*root)++;
	*mp = m;

	return 0;
}
