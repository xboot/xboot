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

static struct runtime_t __runtime = {
	.__errno		= 0,

	.__environ = {
		.content	= "",
		.prev		= &(__runtime.__environ),
		.next		= &(__runtime.__environ),
	},

	.__seed = {
		[0] 		= 1,
		[1] 		= 1,
		[2] 		= 1,
	},
};

struct runtime_t * __get_runtime(void)
{
	return &__runtime;
}

struct runtime_t * runtime_alloc(void)
{
	struct runtime_t * runtime;

	runtime = malloc(sizeof(struct runtime_t));
	if(!runtime)
		return NULL;
	memset(runtime, 0, sizeof(struct runtime_t));

	runtime->__errno = 0;

	runtime->__environ.content = "";
	runtime->__environ.next = &(runtime->__environ);
	runtime->__environ.prev = &(runtime->__environ);

	runtime->__seed[0] = 1;
	runtime->__seed[1] = 1;
	runtime->__seed[2] = 1;

	return runtime;
}

void runtime_free(struct runtime_t * runtime)
{
	if(runtime)
		free(runtime);
}
