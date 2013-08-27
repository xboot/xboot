/*
 * kernel/core/kobj.c
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
#include <spinlock.h>
#include <xboot/kobj.h>

static struct kobj_t * __kobj_root;

static struct kobj_t * __kobj_alloc(const char * name, enum kobj_type_t type, kobj_read_t read, kobj_write_t write, void * priv)
{
	struct kobj_t * kobj;

	if(!name)
		return NULL;

	kobj = malloc(sizeof(struct kobj_t));
	if(!kobj)
		return NULL;

	kobj->name = strdup(name);
	kobj->type = type;
	kobj->parent = kobj;
	init_list_head(&kobj->entry);
	init_list_head(&kobj->children);
	spin_lock_init(&kobj->lock);
	kobj->read = read;
	kobj->write = write;
	kobj->priv = priv;

	return kobj;
}

static void __kobj_free(struct kobj_t * kobj)
{
	if(!kobj)
		return;

	free(kobj->name);
	free(kobj);
}

static struct kobj_t * __kobj_search(struct kobj_t * parent, const char * name)
{
	struct kobj_t * pos, * n;

	if(!parent)
		return NULL;

	if(parent->type != KOBJ_TYPE_DIR)
		return NULL;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(parent->children), entry)
	{
		if(strcmp(pos->name, name) == 0)
			return pos;
	}

	return NULL;
}

static bool_t __kobj_add(struct kobj_t * parent, struct kobj_t * kobj)
{
	if(!parent)
		return FALSE;

	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;

	if(!kobj)
		return FALSE;

	if(__kobj_search(parent, kobj->name))
		return FALSE;

	spin_lock_irq(&parent->lock);
	spin_lock_irq(&kobj->lock);

	kobj->parent = parent;
	list_add_tail(&kobj->entry, &parent->children);

	spin_unlock_irq(&kobj->lock);
	spin_unlock_irq(&parent->lock);

	return TRUE;
}

static bool_t __kobj_remove(struct kobj_t * parent, struct kobj_t * kobj)
{
	struct kobj_t * pos, * n;

	if(!parent)
		return FALSE;

	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;

	if(!kobj)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(parent->children), entry)
	{
		if(pos == kobj)
		{
			spin_lock_irq(&parent->lock);
			spin_lock_irq(&kobj->lock);

			pos->parent = pos;
			list_del(&(pos->entry));

			spin_unlock_irq(&kobj->lock);
			spin_unlock_irq(&parent->lock);

			__kobj_free(kobj);
			return TRUE;
		}
	}

	return FALSE;
}

bool_t kobj_add_directory(struct kobj_t * parent, const char * name)
{
	struct kobj_t * kobj;

	if(!parent)
		return FALSE;

	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;

	if(!name)
		return FALSE;

	if(__kobj_search(parent, name))
		return FALSE;

	kobj = __kobj_alloc(name, KOBJ_TYPE_DIR, NULL, NULL, NULL);
	if(!kobj)
		return FALSE;

	if(!__kobj_add(parent, kobj))
		__kobj_free(kobj);

	return TRUE;
}

bool_t kobj_add_regular(struct kobj_t * parent, const char * name, kobj_read_t read, kobj_write_t write, void * priv)
{
	struct kobj_t * kobj;

	if(!parent)
		return FALSE;

	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;

	if(!name)
		return FALSE;

	if(__kobj_search(parent, name))
		return FALSE;

	kobj = __kobj_alloc(name, KOBJ_TYPE_REG, read, write, priv);
	if(!kobj)
		return FALSE;

	if(!__kobj_add(parent, kobj))
		__kobj_free(kobj);

	return TRUE;
}

bool_t kobj_remove(struct kobj_t * parent, const char * name)
{
	struct kobj_t * kobj;
	struct kobj_t * pos, * n;

	if(!parent)
		return FALSE;

	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;

	if(!name)
		return FALSE;

	kobj = __kobj_search(parent, name);
	if(!kobj)
		return FALSE;

	if(kobj->type == KOBJ_TYPE_DIR)
	{
		list_for_each_entry_safe(pos, n, &(kobj->children), entry)
		{
			kobj_remove(kobj, pos->name);
		}
	}

	return __kobj_remove(parent, kobj);
}

struct kobj_t * kobj_get_root(void)
{
	return __kobj_root;
}

void do_kobj_init(void)
{
	__kobj_root = __kobj_alloc("kobj", KOBJ_TYPE_DIR, NULL, NULL, NULL);
}
