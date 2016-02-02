/*
 * sandbox-logger.c
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
#include <sandbox.h>

static void logger_init(struct logger_t * logger)
{
}

static void logger_output(struct logger_t * logger, const char * buf, int count)
{
	sandbox_console_write((void *)buf, count);
}

static struct logger_t logger = {
	.name	= "logger-sandbox.0",
	.init	= logger_init,
	.output	= logger_output,
};

static __init void sandbox_logger_init(void)
{
	register_logger(&logger);
}
core_initcall(sandbox_logger_init);
