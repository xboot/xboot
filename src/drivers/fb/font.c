/*
 * drivers/fb/font.c
 *
 * Copyright (c) 2007-2010 jianjun jiang <jerryjianjun@gmail.com>
 * website: http://xboot.org
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <malloc.h>
#include <string.h>
#include <xboot/list.h>
#include <fb/font.h>

/* the list of font */
static struct font_list __font_list = {
	.entry = {
		.next	= &(__font_list.entry),
		.prev	= &(__font_list.entry),
	},
};
static struct font_list * font_list = &__font_list;

/*
 * search font by name
 */
static struct font * search_font(const char * name)
{
	struct font_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&font_list->entry)->next; pos != (&font_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct font_list, entry);
		if(strcmp((x_s8*)list->font->name, (const x_s8 *)name) == 0)
			return list->font;
	}

	return NULL;
}

/*
 * register font into font_list
 */
static x_bool register_font(struct font * font)
{
	struct font_list * list;

	list = malloc(sizeof(struct font_list));
	if(!list || !font)
	{
		free(list);
		return FALSE;
	}

	if(!font->name || search_font(font->name))
	{
		free(list);
		return FALSE;
	}

	list->font = font;
	list_add(&list->entry, &font_list->entry);

	return TRUE;
}

/*
 * unregister font from font_list
 */
static x_bool unregister_font(struct font * font)
{
	struct font_list * list;
	struct list_head * pos;

	if(!font || !font->name)
		return FALSE;

	for(pos = (&font_list->entry)->next; pos != (&font_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct font_list, entry);
		if(list->font == font)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

/*
 * load font from file path
 */
x_bool font_load(const char * path)
{
	return FALSE;
}

/*
 * get font by font name
 */
struct font * font_get(const char * name)
{
	return NULL;
}

/*
 * remove font by font name
 */
x_bool font_remove(const char * name)
{
	return FALSE;
}
