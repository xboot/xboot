/*
 * kernel/core/module.c
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

#include <xboot.h>
#include <runtime.h>
#include <xboot/module.h>

extern struct symbol_t __ksymtab_start[];
extern struct symbol_t __ksymtab_end[];

static struct symbol_t * __lookup_symbol_in_range(struct symbol_t * from, struct symbol_t * to, const char * name)
{
	struct symbol_t * next;

	if(!from || !to || !name)
		return NULL;

	next = from;
	while(next < to)
	{
		if(strcmp(next->name, name) == 0)
			return next;
		next++;
	}

	return NULL;
}

static struct symbol_t * __lookup_symbol_in_module(struct module_t * module, const char * name)
{
	struct symbol_t * from, * to;

	if(!module || !name)
		return NULL;

	from = (struct symbol_t *)(module->symtab);
	to = (struct symbol_t *)(module->symtab + module->nsym);

	return __lookup_symbol_in_range(from, to, name);
}

static struct symbol_t * __lookup_symbol_all(const char * name)
{
	struct module_list * m = runtime_get()->__module_list;
	struct module_list * list;
	struct list_head * pos;
	struct symbol_t * sym;

	if(!name)
		return NULL;

	sym = __lookup_symbol_in_range(&(*__ksymtab_start), &(*__ksymtab_end), name);
	if(sym)
		return sym;

	for(pos = (&m->entry)->next; pos != (&m->entry); pos = pos->next)
	{
		list = list_entry(pos, struct module_list, entry);
		sym = __lookup_symbol_in_module(list->module, name);
		if(sym)
			return sym;
	}

	return NULL;
}

void * __symbol_get(const char * name)
{
	struct symbol_t * sym;

	sym = __lookup_symbol_all(name);
	return sym ? (void *)sym->addr : NULL;
}
EXPORT_SYMBOL(__symbol_get);

struct symbol_t * find_symbol(struct module_t * module, const char * name)
{
	return __lookup_symbol_in_module(module, name);
}
EXPORT_SYMBOL(find_symbol);

struct module_t * find_module(const char * name)
{
	struct module_list * m = runtime_get()->__module_list;
	struct module_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&m->entry)->next; pos != (&m->entry); pos = pos->next)
	{
		list = list_entry(pos, struct module_list, entry);
		if(strcmp(list->module->name, name) == 0)
			return list->module;
	}

	return NULL;
}
EXPORT_SYMBOL(find_module);





bool_t add_module(struct module_t * module)
{
	struct module_list * m = runtime_get()->__module_list;
	struct module_list * list;

	list = malloc(sizeof(struct module_list));
	if(!list || !module)
	{
		free(list);
		return FALSE;
	}

	if(!module->name || find_module(module->name))
	{
		free(list);
		return FALSE;
	}

	list->module = module;
	list_add(&list->entry, &m->entry);

	return TRUE;
}

static bool_t delete_module(struct module_t * module)
{
	struct module_list * m = runtime_get()->__module_list;
	struct module_list * list;
	struct list_head * pos;

	if(!module || !module->name)
		return FALSE;

	for(pos = (&m->entry)->next; pos != (&m->entry); pos = pos->next)
	{
		list = list_entry(pos, struct module_list, entry);
		if(list->module == module)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}




struct module_list * __module_list_init(void)
{
	struct module_list * m;

	m = malloc(sizeof(struct module_list));
	if(!m)
		return NULL;

	m->module = NULL;
	init_list_head(&(m->entry));

	return m;
}

void __module_list_exit(struct module_list * m)
{
	if(!m)
		return;

	free(m);
}
