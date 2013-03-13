/*
 * init/runtime.c
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

#include <runtime.h>

static char heap[CONFIG_HEAP_SIZE] __attribute__((__used__, __section__(".heap")));
static struct runtime_t * __current_runtime = NULL;

static void do_runtime_init(void)
{
	static struct runtime_t rt;

	/* Set default runtime to current */
	__current_runtime = &rt;

	/* Initial the default runtime */
	memset(&rt, 0, sizeof(struct runtime_t));

	rt.__pool = memory_pool_create((void *)heap, sizeof(heap));
	rt.__errno = 0;

	rt.__seed[0] = 1;
	rt.__seed[1] = 1;
	rt.__seed[2] = 1;

	rt.__environ.content = "";
	rt.__environ.next = &(rt.__environ);
	rt.__environ.prev = &(rt.__environ);
}

struct runtime_t * runtime_get(void)
{
	return __current_runtime;
}

bool_t runtime_alloc_save(struct runtime_t ** rt)
{
	struct runtime_t * r;

	/* No current runtime */
	if(__current_runtime == NULL)
		do_runtime_init();

	/* Save the current runtime */
	if(rt)
		*rt = __current_runtime;

	/* New runtime */
	r = malloc(sizeof(struct runtime_t));
	if(!r)
		return FALSE;

	r->__pool = __current_runtime->__pool;
	r->__errno = 0;

	r->__seed[0] = 1;
	r->__seed[1] = 1;
	r->__seed[2] = 1;

	r->__environ.content = "";
	r->__environ.next = &(r->__environ);
	r->__environ.prev = &(r->__environ);

	r->__stdin = __file_alloc(0);
	r->__stdout = __file_alloc(1);
	r->__stderr = __file_alloc(2);

	r->__module_list = __module_list_init();
	r->__event_base = __event_base_alloc();
	r->__xfs_ctx = __xfs_alloc();

	/* Set new runtime to current */
	__current_runtime = r;

	return TRUE;
}

bool_t runtime_free_restore(struct runtime_t * rt)
{
	struct runtime_t * r = __current_runtime;

	if(r)
	{
		if(r->__xfs_ctx)
			__xfs_free(r->__xfs_ctx);

		if(r->__event_base)
			__event_base_free(r->__event_base);

		if(r->__module_list)
			__module_list_exit(r->__module_list);

		if(r->__stderr)
			fclose(r->__stderr);

		if(r->__stdout)
			fclose(r->__stdout);

		if(r->__stdin)
			fclose(r->__stdin);

		free(r);
	}

	/* Restore the current runtime */
	if(rt)
		__current_runtime = rt;

	return TRUE;
}
