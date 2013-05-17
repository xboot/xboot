/*
 * xboot/kernel/core/logger.c
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
#include <xboot/logger.h>

struct logger_list_t
{
	struct logger_t * logger;
	struct list_head entry;
};

static struct logger_list_t __logger_list_t = {
	.entry = {
		.next	= &(__logger_list_t.entry),
		.prev	= &(__logger_list_t.entry),
	},
};
struct logger_list_t * logger_list_t = &__logger_list_t;

static struct logger_t * search_logger(const char * name)
{
	struct logger_list_t * list;
	struct list_head * pos;

	if(!name)
		return NULL;

	for(pos = (&logger_list_t->entry)->next; pos != (&logger_list_t->entry); pos = pos->next)
	{
		list = list_entry(pos, struct logger_list_t, entry);
		if(strcmp(list->logger->name, name) == 0)
			return list->logger;
	}

	return NULL;
}

bool_t register_logger(struct logger_t * logger)
{
	struct logger_list_t * list;

	list = malloc(sizeof(struct logger_list_t));
	if(!list || !logger)
	{
		free(list);
		return FALSE;
	}

	if(!logger->name || search_logger(logger->name))
	{
		free(list);
		return FALSE;
	}

	if(logger->init)
		(logger->init)();

	if(logger->output)
		logger->output((const u8_t *)xboot_banner_string(), strlen(xboot_banner_string()));

	list->logger = logger;
	list_add(&list->entry, &logger_list_t->entry);

	return TRUE;
}

bool_t unregister_logger(struct logger_t * logger)
{
	struct logger_list_t * list;
	struct list_head * pos;

	if(!logger || !logger->name)
		return FALSE;

	for(pos = (&logger_list_t->entry)->next; pos != (&logger_list_t->entry); pos = pos->next)
	{
		list = list_entry(pos, struct logger_list_t, entry);
		if(list->logger == logger)
		{
			if(logger->exit)
				(logger->exit)();

			list_del(pos);
			free(list);
			return TRUE;
		}
	}

	return FALSE;
}

int logger_output(const char * file, const int line, const char * fmt, ...)
{
	struct logger_list_t * list;
	struct list_head * pos;
	va_list ap;
	char * p;
	int len = 0;
	int div, rem;

	if((p = malloc(SZ_4K)) == NULL)
		return 0;

	va_start(ap, fmt);
	if(get_system_hz() > 0)
	{
		div = jiffies * 1000000 / get_system_hz() / 1000000;
		rem = jiffies * 1000000 / get_system_hz() % 1000000;
	}
	else
	{
		div = 0;
		rem = 0;
	}
	len += sprintf((char *)(p + len), (const char *)"[%5u.%06u]", div, rem);
	len += sprintf((char *)(p + len), (const char *)"[%s:%d] ", file, line);
	len += vsnprintf((char *)(p + len), (SZ_4K - len), fmt, ap);
	len += sprintf((char *)(p + len), "\r\n");
	va_end(ap);

	for(pos = (&logger_list_t->entry)->next; pos != (&logger_list_t->entry); pos = pos->next)
	{
		list = list_entry(pos, struct logger_list_t, entry);
		if(list->logger->output)
			list->logger->output((const u8_t *)p, len);
	}

	free(p);
	return len;
}
EXPORT_SYMBOL(logger_output);
