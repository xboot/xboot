/*
 * init/runtime.c
 *
 * Copyright (c) 2007-2011  jianjun jiang <jerryjianjun@gmail.com>
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

#include <xboot/module.h>
#include <runtime.h>

static char heap[CONFIG_HEAP_SIZE] __attribute__((__used__, __section__(".heap")));
static struct runtime_t * __runtime = NULL;

static struct runtime_t * strap_runtime_init(void)
{
	static struct runtime_t strap;

	memset(&strap, 0, sizeof(struct runtime_t));
	strap.__pool = memory_pool_create((void *)heap, sizeof(heap));

	return &strap;
}

void __set_runtime(struct runtime_t * r)
{
	if(r)
		__runtime = r;
}
EXPORT_SYMBOL(__set_runtime);

struct runtime_t * __get_runtime(void)
{
	return __runtime;
}
EXPORT_SYMBOL(__get_runtime);

struct runtime_t * runtime_alloc(void)
{
	struct runtime_t * r;

	/*
	 * No current runtime, Set to strap runtime.
	 */
	if(__get_runtime() == NULL)
		__set_runtime(strap_runtime_init());

	r = malloc(sizeof(struct runtime_t));
	if(!r)
		return NULL;
	memset(r, 0, sizeof(struct runtime_t));

	r->__pool = __get_runtime()->__pool;
	r->__errno = 0;

	r->__environ.content = "";
	r->__environ.next = &(r->__environ);
	r->__environ.prev = &(r->__environ);

	r->__seed[0] = 1;
	r->__seed[1] = 1;
	r->__seed[2] = 1;

	r->__stdin = __file_alloc(0);
	r->__stdout = __file_alloc(1);
	r->__stderr = __file_alloc(2);

	r->__xfs_ctx = __xfs_platform_init();
	r->__module_list = __module_list_init();

	return r;
}
EXPORT_SYMBOL(runtime_alloc);

void runtime_free(struct runtime_t * r)
{
	if(!r)
		return;

	if(r->__stdin)
		fclose(r->__stdin);

	if(r->__stdout)
		fclose(r->__stdout);

	if(r->__stderr)
		fclose(r->__stderr);

	if(r->__xfs_ctx)
		__xfs_platform_exit(r->__xfs_ctx);

	if(r->__module_list)
		__module_list_exit(r->__module_list);

	if(r)
		free(r);
}
EXPORT_SYMBOL(runtime_free);
