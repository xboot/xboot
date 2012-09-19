/*
 * kernel/core/module.c
 *
 * Copyright (c) 2007-2012  jianjun jiang <jerryjianjun@gmail.com>
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

#include <xboot.h>
#include <xboot/module.h>

static struct kernel_symbol * __find_symbol(const char * name)
{
	struct kernel_symbol * next;

	if(!name)
		return NULL;

	next = &(*__ksymtab_start);
	while (next < &(*__ksymtab_end))
	{
		if(strcmp(next->name, name) == 0)
			return next;
		next++;
	}

	return NULL;
}

void * __symbol_get(const char * name)
{
	struct kernel_symbol * sym;

	sym = __find_symbol(name);
	return sym ? (void *)sym->addr : NULL;
}
EXPORT_SYMBOL(__symbol_get);
