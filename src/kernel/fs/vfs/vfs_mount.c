/*
 * kernel/fs/vfs/vfs_mount.c
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
