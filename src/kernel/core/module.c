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

static struct list_head __module_list = {
	.next = &__module_list,
	.prev = &__module_list,
};
static spinlock_t __module_lock = SPIN_LOCK_INIT();

static struct kobj_t * search_class_module_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "module");
}

static ssize_t module_read_ksymtab(struct kobj_t * kobj, void * buf, size_t size)
{
	struct symbol_t * from = &(*__ksymtab_start);
	struct symbol_t * to = &(*__ksymtab_end);
	struct symbol_t * next;
	char * p = buf;
	int len = 0;

	next = from;
	while(next < to)
	{
		len += sprintf((char *)(p + len), "%p - %s\r\n", next->addr, next->name);
		next++;
	}
	return len;
}

static ssize_t module_read_symtab(struct kobj_t * kobj, void * buf, size_t size)
{
	struct module_t * m = (struct module_t *)kobj->priv;
	struct symbol_t * from = (struct symbol_t *)(m->symtab);
	struct symbol_t * to = (struct symbol_t *)(m->symtab + m->nsym);
	struct symbol_t * next;
	char * p = buf;
	int len = 0;

	next = from;
	while(next < to)
	{
		len += sprintf((char *)(p + len), "%p - %s\r\n", next->addr, next->name);
		next++;
	}
	return len;
}

static struct module_t * search_module(const char * name)
{
	struct module_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &__module_list, list)
	{
		if(strcmp(pos->name, name) == 0)
			return pos;
	}
	return NULL;
}

static struct symbol_t * lookup_symbol_in_range(struct symbol_t * from, struct symbol_t * to, const char * name)
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

static struct symbol_t * lookup_symbol_in_module(struct module_t * m, const char * name)
{
	struct symbol_t * from, * to;

	if(!m || !name)
		return NULL;

	from = (struct symbol_t *)(m->symtab);
	to = (struct symbol_t *)(m->symtab + m->nsym);

	return lookup_symbol_in_range(from, to, name);
}

static struct symbol_t * lookup_symbol_all(const char * name)
{
	struct module_t * pos, * n;
	struct symbol_t * sym;

	if(!name)
		return NULL;

	sym = lookup_symbol_in_range(&(*__ksymtab_start), &(*__ksymtab_end), name);
	if(sym)
		return sym;

	list_for_each_entry_safe(pos, n, &__module_list, list)
	{
		sym = lookup_symbol_in_module(pos, name);
		if(sym)
			return sym;
	}
	return NULL;
}

void * __symbol_get(const char * name)
{
	struct symbol_t * sym;

	sym = lookup_symbol_all(name);
	return sym ? (void *)sym->addr : NULL;
}
EXPORT_SYMBOL(__symbol_get);

bool_t register_module(struct module_t * m)
{
	irq_flags_t flags;

	if(!m || !m->name)
		return FALSE;

	if(search_module(m->name))
		return FALSE;

	m->kobj = kobj_alloc_directory(m->name);
	kobj_add_regular(m->kobj, "symtab", module_read_symtab, NULL, m);
	kobj_add(search_class_module_kobj(), m->kobj);

	spin_lock_irqsave(&__module_lock, flags);
	init_list_head(&m->list);
	list_add_tail(&m->list, &__module_list);
	spin_unlock_irqrestore(&__module_lock, flags);

	return TRUE;
}
EXPORT_SYMBOL(register_module);

bool_t unregister_module(struct module_t * m)
{
	irq_flags_t flags;

	if(!m || !m->name)
		return FALSE;

	spin_lock_irqsave(&__module_lock, flags);
	list_del(&m->list);
	spin_unlock_irqrestore(&__module_lock, flags);
	kobj_remove(search_class_module_kobj(), m->kobj);
	kobj_remove_self(m->kobj);

	return TRUE;
}
EXPORT_SYMBOL(unregister_module);

static __init void module_init(void)
{
	kobj_add_regular(search_class_module_kobj(), "ksymtab", module_read_ksymtab, NULL, NULL);
}
core_initcall(module_init);
