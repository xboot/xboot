/*
 * drivers/led/trigger.c
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
#include <xboot/list.h>
#include <xboot/printk.h>
#include <xboot/initcall.h>
#include <xboot/resource.h>
#include <time/timer.h>
#include <led/led.h>
#include <led/trigger.h>

/*
 * the list of led trigger
 */
static struct led_trigger_list __led_trigger_list = {
	.entry = {
		.next	= &(__led_trigger_list.entry),
		.prev	= &(__led_trigger_list.entry),
	},
};
struct led_trigger_list * led_trigger_list = &__led_trigger_list;

/*
 * search led trigger by name. a static function.
 */
static struct trigger * trigger_search(const char *name)
{
	struct led_trigger_list * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&led_trigger_list->entry)->next; pos != (&led_trigger_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct led_trigger_list, entry);
		if(strcmp((char*)list->trigger->name, (const char *)name) == 0)
			return list->trigger;
	}

	return NULL;
}

/*
 * register a trigger into led_trigger_list
 * return true is successed, otherwise is not.
 */
bool_t trigger_register(struct trigger * trigger)
{
	struct led_trigger_list * list;

	list = malloc(sizeof(struct led_trigger_list));
	if(!list || !trigger)
	{
		free(list);
		return FALSE;
	}

	if(!trigger->name || !trigger->led || trigger_search(trigger->name))
	{
		free(list);
		return FALSE;
	}

	if(trigger->activate)
		(trigger->activate)(trigger);

	list->trigger = trigger;
	list_add(&list->entry, &led_trigger_list->entry);

	return TRUE;
}

/*
 * unregister trigger from led_trigger_hash
 */
bool_t trigger_unregister(struct trigger * trigger)
{
	struct led_trigger_list * list;
	struct list_head * pos;

	if(!trigger || !trigger->name)
		return FALSE;

	for(pos = (&led_trigger_list->entry)->next; pos != (&led_trigger_list->entry); pos = pos->next)
	{
		list = list_entry(pos, struct led_trigger_list, entry);
		if(list->trigger == trigger)
		{
			if(trigger->deactivate)
				(trigger->deactivate)(trigger);

			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}
