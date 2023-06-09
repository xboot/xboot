/*
 * driver/block/partition/partition.c
 *
 * Copyright(c) 2007-2023 Jianjun Jiang <8192542@qq.com>
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
#include <block/partition.h>

static struct list_head __partition_parser_list = {
	.next = &__partition_parser_list,
	.prev = &__partition_parser_list,
};
static spinlock_t __partition_parser_lock = SPIN_LOCK_INIT();

static struct kobj_t * search_class_partition_kobj(void)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	return kobj_search_directory_with_create(kclass, "partition");
}

static struct partition_parser_t * search_partition_parser(const char * name)
{
	struct partition_parser_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &__partition_parser_list, list)
	{
		if(strcmp(pos->name, name) == 0)
			return pos;
	}
	return NULL;
}

bool_t register_partition_parser(struct partition_parser_t * parser)
{
	irq_flags_t flags;

	if(!parser || !parser->name || !parser->parse)
		return FALSE;

	if(search_partition_parser(parser->name))
		return FALSE;

	parser->kobj = kobj_alloc_directory(parser->name);
	kobj_add(search_class_partition_kobj(), parser->kobj);

	spin_lock_irqsave(&__partition_parser_lock, flags);
	init_list_head(&parser->list);
	list_add_tail(&parser->list, &__partition_parser_list);
	spin_unlock_irqrestore(&__partition_parser_lock, flags);

	return TRUE;
}

bool_t unregister_partition_parser(struct partition_parser_t * parser)
{
	irq_flags_t flags;

	if(!parser || !parser->name)
		return FALSE;

	spin_lock_irqsave(&__partition_parser_lock, flags);
	list_del(&parser->list);
	spin_unlock_irqrestore(&__partition_parser_lock, flags);
	kobj_remove(search_class_partition_kobj(), parser->kobj);
	kobj_remove_self(parser->kobj);

	return TRUE;
}

void partition_parse(struct block_t * pblk)
{
	struct partition_parser_t * pos, * n;

	if(pblk && pblk->name && (block_capacity(pblk) > 0))
	{
		list_for_each_entry_safe(pos, n, &__partition_parser_list, list)
		{
			if(pos->parse(pblk))
				break;
		}
	}
}
