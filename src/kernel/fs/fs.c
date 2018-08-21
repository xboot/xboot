/*
 * kernel/fs/fs.c
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
#include <fs/fs.h>

/*
 * the list of filesystem
 */
static struct fs_list __fs_list = {
	.entry = {
		.next	= &(__fs_list.entry),
		.prev	= &(__fs_list.entry),
	},
};
struct fs_list * fs_list = &__fs_list;

/*
 * search filesystem by name
 */
struct filesystem_t * filesystem_search(const char * name)
{
	struct fs_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&fs_list->entry)->next; pos != (&fs_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct fs_list, entry);
		if(strcmp(list->fs->name, name) == 0)
			return list->fs;
	}

	return NULL;
}

/*
 * register a filesystem into fs_list
 */
bool_t filesystem_register(struct filesystem_t * fs)
{
	struct fs_list * list;

	list = malloc(sizeof(struct fs_list));
	if(!list || !fs)
	{
		free(list);
		return FALSE;
	}

	if(!fs->name || filesystem_search(fs->name))
	{
		free(list);
		return FALSE;
	}

	list->fs = fs;
	list_add(&list->entry, &fs_list->entry);

	return TRUE;
}

/*
 * unregister a filesystem from fs_list
 */
bool_t filesystem_unregister(struct filesystem_t * fs)
{
	struct fs_list * list;
	struct list_head * pos;

	if(!fs || !fs->name)
		return FALSE;

	for(pos = (&fs_list->entry)->next; pos != (&fs_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct fs_list, entry);
		if(list->fs == fs)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}
