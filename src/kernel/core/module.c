/*
 * kernel/core/module.c
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
#include <xboot/module.h>

extern struct symbol_t __ksymtab_start[];
extern struct symbol_t __ksymtab_end[];

struct module_list_t
{
	struct module_t * module;
	struct list_head entry;
};

static struct module_list_t __module_list = {
	.entry = {
		.next	= &(__module_list.entry),
		.prev	= &(__module_list.entry),
	},
};
static spinlock_t __module_list_lock = SPIN_LOCK_INIT();

static struct module_t * search_module(const char * name)
{
	struct module_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__module_list.entry), entry)
	{
		if(strcmp(pos->module->name, name) == 0)
			return pos->module;
	}

	return NULL;
}

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
	struct module_list_t * pos, * n;
	struct symbol_t * sym;

	if(!name)
		return NULL;

	sym = __lookup_symbol_in_range(&(*__ksymtab_start), &(*__ksymtab_end), name);
	if(sym)
		return sym;

	list_for_each_entry_safe(pos, n, &(__module_list.entry), entry)
	{
		sym = __lookup_symbol_in_module(pos->module, name);
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

bool_t register_module(struct module_t * module)
{
	struct module_list_t * ml;
	irq_flags_t flags;

	if(!module || !module->name)
		return FALSE;

	if(search_module(module->name))
		return FALSE;

	ml = malloc(sizeof(struct module_list_t));
	if(!ml)
		return FALSE;

	ml->module = module;

	spin_lock_irqsave(&__module_list_lock, flags);
	list_add_tail(&ml->entry, &(__module_list.entry));
	spin_unlock_irqrestore(&__module_list_lock, flags);

	return TRUE;
}
EXPORT_SYMBOL(register_module);

bool_t unregister_module(struct module_t * module)
{
	struct module_list_t * pos, * n;
	irq_flags_t flags;

	if(!module || !module->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__module_list.entry), entry)
	{
		if(pos->module == module)
		{
			spin_lock_irqsave(&__module_list_lock, flags);
			list_del(&(pos->entry));
			spin_unlock_irqrestore(&__module_list_lock, flags);

			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}
EXPORT_SYMBOL(unregister_module);
