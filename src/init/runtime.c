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

#include <runtime.h>

static struct runtime_t * __runtime = NULL;

struct runtime_t * __get_runtime(void)
{
	return __runtime;
}

void __set_runtime(struct runtime_t * r)
{
	__runtime = r;
}

struct runtime_t * runtime_alloc(void)
{
	struct runtime_t * r;

	r = malloc(sizeof(struct runtime_t));
	if(!r)
		return NULL;
	memset(r, 0, sizeof(struct runtime_t));

	r->__errno = 0;

	r->__environ.content = "";
	r->__environ.next = &(r->__environ);
	r->__environ.prev = &(r->__environ);

	r->__seed[0] = 1;
	r->__seed[1] = 1;
	r->__seed[2] = 1;

//TODO	__stdio_init(r);

	return r;
}

void runtime_free(struct runtime_t * r)
{
	if(r)
		free(r);
}
