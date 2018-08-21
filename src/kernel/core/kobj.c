/*
 * kernel/core/kobj.c
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
#include <spinlock.h>
#include <xboot/kobj.h>

static struct kobj_t * __kobj_root = NULL;

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

struct kobj_t * kobj_get_root(void)
{
	if(!__kobj_root)
		__kobj_root = kobj_alloc_directory("kobj");
	return __kobj_root;
}

struct kobj_t * kobj_search(struct kobj_t * parent, const char * name)
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

struct kobj_t * kobj_search_directory_with_create(struct kobj_t * parent, const char * name)
{
	struct kobj_t * kobj;

	if(!parent)
		return NULL;

	if(parent->type != KOBJ_TYPE_DIR)
		return NULL;

	if(!name)
		return NULL;

	kobj = kobj_search(parent, name);
	if(!kobj)
	{
		kobj = kobj_alloc_directory(name);
		if(!kobj)
			return NULL;

		if(!kobj_add(parent, kobj))
		{
			kobj_free(kobj);
			return NULL;
		}
	}
	else if(kobj->type != KOBJ_TYPE_DIR)
	{
		return NULL;
	}

	return kobj;
}

struct kobj_t * kobj_alloc_directory(const char * name)
{
	return __kobj_alloc(name, KOBJ_TYPE_DIR, NULL, NULL, NULL);
}

struct kobj_t * kobj_alloc_regular(const char * name, kobj_read_t read, kobj_write_t write, void * priv)
{
	return __kobj_alloc(name, KOBJ_TYPE_REG, read, write, priv);
}

bool_t kobj_free(struct kobj_t * kobj)
{
	if(!kobj)
		return FALSE;

	free(kobj->name);
	free(kobj);
	return TRUE;
}

bool_t kobj_add(struct kobj_t * parent, struct kobj_t * kobj)
{
	irq_flags_t pflags, flags;

	if(!parent)
		return FALSE;

	if(parent->type != KOBJ_TYPE_DIR)
		return FALSE;

	if(!kobj)
		return FALSE;

	if(kobj_search(parent, kobj->name))
		return FALSE;

	spin_lock_irqsave(&parent->lock, pflags);
	spin_lock_irqsave(&kobj->lock, flags);

	kobj->parent = parent;
	list_add_tail(&kobj->entry, &parent->children);

	spin_unlock_irqrestore(&kobj->lock, flags);
	spin_unlock_irqrestore(&parent->lock, pflags);

	return TRUE;
}

bool_t kobj_remove(struct kobj_t * parent, struct kobj_t * kobj)
{
	struct kobj_t * pos, * n;
	irq_flags_t pflags, flags;

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
			spin_lock_irqsave(&parent->lock, pflags);
			spin_lock_irqsave(&kobj->lock, flags);

			pos->parent = pos;
			list_del(&(pos->entry));

			spin_unlock_irqrestore(&kobj->lock, flags);
			spin_unlock_irqrestore(&parent->lock, pflags);

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

	if(kobj_search(parent, name))
		return FALSE;

	kobj = kobj_alloc_directory(name);
	if(!kobj)
		return FALSE;

	if(!kobj_add(parent, kobj))
		kobj_free(kobj);

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

	if(kobj_search(parent, name))
		return FALSE;

	kobj = kobj_alloc_regular(name, read, write, priv);
	if(!kobj)
		return FALSE;

	if(!kobj_add(parent, kobj))
		kobj_free(kobj);

	return TRUE;
}

bool_t kobj_remove_self(struct kobj_t * kobj)
{
	struct kobj_t * parent;
	struct kobj_t * pos, * n;
	bool_t ret;

	if(!kobj)
		return FALSE;

	if(kobj->type == KOBJ_TYPE_DIR)
	{
		list_for_each_entry_safe(pos, n, &(kobj->children), entry)
		{
			kobj_remove_self(pos);
		}
	}

	parent = kobj->parent;
	if(parent && (parent != kobj))
	{
		ret = kobj_remove(parent, kobj);
		if(ret)
			kobj_free(kobj);
		return ret;
	}

	kobj_free(kobj);
	return TRUE;
}
