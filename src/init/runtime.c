/*
 * init/runtime.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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

#include <xfs/xfs.h>
#include <runtime.h>

static struct runtime_t * __current_runtime = NULL;

struct runtime_t * runtime_get(void)
{
	return __current_runtime;
}

void runtime_create_save(struct runtime_t * rt, const char * path, struct runtime_t ** r)
{
	if(!rt)
		return;

	if(r)
		*r = __current_runtime;
	__current_runtime = rt;

	rt->__errno = 0;

	rt->__seed[0] = 1;
	rt->__seed[1] = 1;
	rt->__seed[2] = 1;

	rt->__environ.content = "";
	rt->__environ.next = &(rt->__environ);
	rt->__environ.prev = &(rt->__environ);

	rt->__stdin = __file_alloc(0);
	rt->__stdout = __file_alloc(1);
	rt->__stderr = __file_alloc(2);

	rt->__event_base = __event_base_alloc();
	rt->__xfs_ctx = __xfs_alloc(path);
}

void runtime_destroy_restore(struct runtime_t * rt, struct runtime_t * r)
{
	if(!rt)
		return;

	if(rt->__xfs_ctx)
		__xfs_free(rt->__xfs_ctx);

	if(rt->__event_base)
		__event_base_free(rt->__event_base);

	if(rt->__stderr)
		fclose(rt->__stderr);

	if(rt->__stdout)
		fclose(rt->__stdout);

	if(rt->__stdin)
		fclose(rt->__stdin);

	if(r)
		__current_runtime = r;
}
