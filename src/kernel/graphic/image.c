/*
 * kernel/graphic/image.c
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

#include <xboot/module.h>
#include <graphic/image.h>

static struct image_loader_list __image_loader_list = {
	.entry = {
		.next	= &(__image_loader_list.entry),
		.prev	= &(__image_loader_list.entry),
	},
};
static struct image_loader_list * image_loader_list = &__image_loader_list;

static bool_t match_extension(const char * filename, const char * ext)
{
	s32_t pos, ext_len;

	pos = strlen(filename);
	ext_len = strlen(ext);

	if( (!pos) || (!ext_len) || (ext_len > pos) )
		return FALSE;

	pos -= ext_len;

	return (strcasecmp((const char *)(filename + pos), (const char *)ext) == 0);
}

static struct image_loader * search_image_loader(const char * extension)
{
	struct image_loader_list * list;
	struct list_head * pos;

	if(!extension)
		return NULL;

	for(pos = (&image_loader_list->entry)->next; pos != (&image_loader_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct image_loader_list, entry);
		if(strcmp((const char *)list->loader->extension, (const char *)extension) == 0)
			return list->loader;
	}

	return NULL;
}

bool_t register_image_loader(struct image_loader * loader)
{
	struct image_loader_list * list;

	list = malloc(sizeof(struct image_loader_list));
	if(!list || !loader)
	{
		free(list);
		return FALSE;
	}

	if(!loader->extension || search_image_loader(loader->extension))
	{
		free(list);
		return FALSE;
	}

	list->loader = loader;
	list_add(&list->entry, &image_loader_list->entry);

	return TRUE;
}

bool_t unregister_image_loader(struct image_loader * loader)
{
	struct image_loader_list * list;
	struct list_head * pos;

	if(!loader || !loader->extension)
		return FALSE;

	for(pos = (&image_loader_list->entry)->next; pos != (&image_loader_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct image_loader_list, entry);
		if(list->loader == loader)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

struct surface_t * surface_load_from_file(const char * filename)
{
	struct image_loader_list * list;
	struct list_head * pos;

	for(pos = (&image_loader_list->entry)->next; pos != (&image_loader_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct image_loader_list, entry);
		if(list->loader->load)
		{
			if(match_extension(filename, list->loader->extension))
				return list->loader->load(filename);
		}
	}

	return NULL;
}
EXPORT_SYMBOL(surface_load_from_file);
