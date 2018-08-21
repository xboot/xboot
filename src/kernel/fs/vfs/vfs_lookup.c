/*
 * kernel/fs/vfs/vfs_lookup.c
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
#include <errno.h>
#include <xboot/initcall.h>
#include <fs/fs.h>
#include <fs/vfs/vfs.h>


/*
 * convert a full path name into a pointer to a locked vnode.
 */
s32_t vfs_namei(char * path, struct vnode_t ** vpp)
{
	char *p;
	char node[MAX_PATH];
	char name[MAX_PATH];
	struct mount_t * mp;
	struct vnode_t * dvp, * vp;
	s32_t error, i;

	/*
	 * convert a full path name to its mount point and
	 * the local node in the file system.
	 */
	if(vfs_findroot(path, &mp, &p))
		return ENOTDIR;

	strlcpy(node, "/", sizeof(node));
	strlcat(node, p, sizeof(node));
	vp = vn_lookup(mp, node);
	if(vp)
	{
		/* vnode is already active */
		*vpp = vp;
		return 0;
	}

	/*
	 * find target vnode, started from root directory.
	 * this is done to attach the fs specific data to
	 * the target vnode.
	 */
	if((dvp = mp->m_root) == NULL)
		return ENOSYS;

	vref(dvp);
	node[0] = '\0';

	while(*p != '\0')
	{
		/*
		 * get lower directory or file name.
		 */
		while(*p == '/')
			p++;

		for(i = 0; i < MAX_PATH; i++)
		{
			if(*p == '\0' || *p == '/')
				break;
			name[i] = *p++;
		}
		name[i] = '\0';

		/*
		 * get a vnode for the target.
		 */
		strlcat(node, "/", sizeof(node));
		strlcat(node, name, sizeof(node));
		vp = vn_lookup(mp, node);
		if(vp == NULL)
		{
			vp = vget(mp, node);
			if(vp == NULL)
			{
				vput(dvp);
				return ENOMEM;
			}

			/*
			 * find a vnode in this directory.
			 */
			error = dvp->v_op->vop_lookup(dvp, name, vp);
			if(error || (*p == '/' && vp->v_type != VDIR))
			{
				/* not found */
				vput(vp);
				vput(dvp);
				return error;
			}
		}

		vput(dvp);
		dvp = vp;
		while(*p != '\0' && *p != '/')
			p++;
	}

	*vpp = vp;

	return 0;
}

/*
 * search a pathname.
 * this is a very central but not so complicated routine.
 *
 * @path: full path.
 * @vpp:  pointer to locked vnode for directory.
 * @name: pointer to file name in path.
 */
s32_t vfs_lookup(char * path, struct vnode_t ** vpp, char ** name)
{
	char buf[MAX_PATH];
	char root[] = "/";
	char *file, *dir;
	struct vnode_t * vp;
	s32_t error;

	/*
	 * get the path for directory.
	 */
	strlcpy(buf, path, sizeof(buf));
	file = strrchr(buf, '/');

	if(!buf[0])
		return ENOTDIR;
	if(file == buf)
		dir = root;
	else
	{
		*file = '\0';
		dir = buf;
	}

	/*
	 * get the vnode for directory
	 */
	if((error = vfs_namei(dir, &vp)) != 0)
		return error;
	if (vp->v_type != VDIR)
	{
		vput(vp);
		return ENOTDIR;
	}
	*vpp = vp;

	/*
	 * get the file name
	 */
	*name = strrchr(path, '/') + 1;

	return 0;
}
