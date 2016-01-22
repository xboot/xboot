/*
 * kernel/command/command.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
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
#include <command/command.h>

struct command_list_t __command_list = {
	.entry = {
		.next	= &(__command_list.entry),
		.prev	= &(__command_list.entry),
	},
};
static spinlock_t __command_list_lock = SPIN_LOCK_INIT();

struct command_t * search_command(const char * name)
{
	struct command_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__command_list.entry), entry)
	{
		if(strcmp(pos->cmd->name, name) == 0)
			return pos->cmd;
	}

	return NULL;
}

bool_t register_command(struct command_t * cmd)
{
	struct command_list_t * cl;
	irq_flags_t flags;

	if(!cmd || !cmd->name)
		return FALSE;

	if(!cmd->exec)
		return FALSE;

	if(search_command(cmd->name))
		return FALSE;

	cl = malloc(sizeof(struct command_list_t));
	if(!cl)
		return FALSE;

	cl->cmd = cmd;

	spin_lock_irqsave(&__command_list_lock, flags);
	list_add_tail(&cl->entry, &(__command_list.entry));
	spin_unlock_irqrestore(&__command_list_lock, flags);

	return TRUE;
}

bool_t unregister_command(struct command_t * cmd)
{
	struct command_list_t * pos, * n;
	irq_flags_t flags;

	if(!cmd || !cmd->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__command_list.entry), entry)
	{
		if(pos->cmd == cmd)
		{
			spin_lock_irqsave(&__command_list_lock, flags);
			list_del(&(pos->entry));
			spin_unlock_irqrestore(&__command_list_lock, flags);

			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

int total_command_number(void)
{
	struct list_head * pos = (&__command_list.entry)->next;
	int i = 0;

	while(!list_is_last(pos, (&__command_list.entry)->next))
	{
		pos = pos->next;
		i++;
	}

	return i;
}
