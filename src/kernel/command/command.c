/*
 * kernel/command/command.c
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
