/*
 * sandboxlinux-logger.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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
#include <sandboxlinux.h>

static void logger_linux_init(void)
{
}

static void logger_linux_exit(void)
{
}

static ssize_t logger_linux_output(const char * buf, size_t count)
{
	return sandbox_linux_console_write((void *)buf, count);
}

static struct logger_t linux_logger = {
	.name	= "logger-linux",
	.init	= logger_linux_init,
	.exit	= logger_linux_exit,
	.output	= logger_linux_output,
};

static __init void linux_logger_init(void)
{
	if(register_logger(&linux_logger))
		LOG("Register logger '%s'", linux_logger.name);
	else
		LOG("Failed to register logger '%s'", linux_logger.name);
}

static __exit void linux_logger_exit(void)
{
	if(unregister_logger(&linux_logger))
		LOG("Unregister logger '%s'", linux_logger.name);
	else
		LOG("Failed to unregister logger '%s'", linux_logger.name);
}

pure_initcall(linux_logger_init);
pure_exitcall(linux_logger_exit);
