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
#include <spinlock.h>
#include <xboot/logger.h>

struct logger_list_t
{
	struct logger_t * logger;
	struct list_head entry;
};

static struct logger_list_t __logger_list = {
	.entry = {
		.next	= &(__logger_list.entry),
		.prev	= &(__logger_list.entry),
	},
};
static spinlock_t __logger_list_lock = SPIN_LOCK_INIT();

static struct logger_t * search_logger(const char * name)
{
	struct logger_list_t * pos, * n;

	if(!name)
		return NULL;

	list_for_each_entry_safe(pos, n, &(__logger_list.entry), entry)
	{
		if(strcmp(pos->logger->name, name) == 0)
			return pos->logger;
	}

	return NULL;
}

bool_t register_logger(struct logger_t * logger)
{
	struct logger_list_t * ll;

	if(!logger || !logger->name)
		return FALSE;

	if(search_logger(logger->name))
		return FALSE;

	ll = malloc(sizeof(struct logger_list_t));
	if(!ll)
		return FALSE;

	if(logger->init)
		(logger->init)();

	if(logger->output)
		logger->output(xboot_banner_string(), strlen(xboot_banner_string()));

	ll->logger = logger;

	spin_lock_irq(&__logger_list_lock);
	list_add_tail(&ll->entry, &(__logger_list.entry));
	spin_unlock_irq(&__logger_list_lock);

	return TRUE;
}

bool_t unregister_logger(struct logger_t * logger)
{
	struct logger_list_t * pos, * n;

	if(!logger || !logger->name)
		return FALSE;

	list_for_each_entry_safe(pos, n, &(__logger_list.entry), entry)
	{
		if(pos->logger == logger)
		{
			if(logger->exit)
				(logger->exit)();

			spin_lock_irq(&__logger_list_lock);
			list_del(&(pos->entry));
			spin_unlock_irq(&__logger_list_lock);

			free(pos);
			return TRUE;
		}
	}

	return FALSE;
}

void logger_output(const char * buf, size_t count)
{
	struct logger_list_t * pos, * n;

	list_for_each_entry_safe(pos, n, &(__logger_list.entry), entry)
	{
		if(pos->logger->output)
			pos->logger->output(buf, count);
	}
}
EXPORT_SYMBOL(logger_output);

int logger_print(const char * fmt, ...)
{
	va_list ap;
	struct timeval tv;
	char buf[SZ_4K];
	int len = 0;

	va_start(ap, fmt);
	gettimeofday(&tv, 0);
	len += sprintf((char *)(buf + len), "[%5u.%06u]", tv.tv_sec, tv.tv_usec % 1000000);
	len += vsnprintf((char *)(buf + len), (SZ_4K - len), fmt, ap);
	va_end(ap);

	logger_output((const char *)buf, len);
	return len;
}
EXPORT_SYMBOL(logger_print);
