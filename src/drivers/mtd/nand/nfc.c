/*
 * driver/mtd/nand/nfc.c
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
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <hash.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/printk.h>
#include <mtd/nand/nand.h>
#include <mtd/nand/nfc.h>


/*
 * the list of nand flash controller
 */
static struct nfc_list __nfc_list = {
	.entry = {
		.next	= &(__nfc_list.entry),
		.prev	= &(__nfc_list.entry),
	},
};
struct nfc_list * nfc_list = &__nfc_list;

/*
 * search nfc by name
 */
struct nfc * search_nfc(const char * name)
{
	struct nfc_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&nfc_list->entry)->next; pos != (&nfc_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct nfc_list, entry);
		if(strcmp(list->nfc->name, name) == 0)
			return list->nfc;
	}

	return NULL;
}

/*
 * register a nfc into nfc_list
 */
bool_t register_nfc(struct nfc * nfc)
{
	struct nfc_list * list;

	list = malloc(sizeof(struct nfc_list));
	if(!list || !nfc)
	{
		free(list);
		return FALSE;
	}

	if(!nfc->name || search_nfc(nfc->name))
	{
		free(list);
		return FALSE;
	}

	list->nfc = nfc;
	list_add(&list->entry, &nfc_list->entry);

	return TRUE;
}

/*
 * unregister nfc from nfc_list
 */
bool_t unregister_nfc(struct nfc * nfc)
{
	struct nfc_list * list;
	struct list_head * pos;

	if(!nfc || !nfc->name)
		return FALSE;

	for(pos = (&nfc_list->entry)->next; pos != (&nfc_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct nfc_list, entry);
		if(list->nfc == nfc)
		{
			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}
