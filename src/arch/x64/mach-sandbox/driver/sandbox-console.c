/*
 * driver/sandbox-console.c
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

#include <console/console.h>
#include <sandbox.h>

static ssize_t stdio_console_read(struct console_t * console, unsigned char * buf, size_t count)
{
	return sandbox_console_read((void *)buf, count);
}

static ssize_t stdio_console_write(struct console_t * console, const unsigned char * buf, size_t count)
{
	return sandbox_console_write((void *)buf, count);
}

static void stdio_console_suspend(struct console_t * console)
{
}

static void stdio_console_resume(struct console_t * console)
{
}

static struct console_t stdio = {
	.name		= "stdio",
	.read		= stdio_console_read,
	.write		= stdio_console_write,
	.suspend	= stdio_console_suspend,
	.resume		= stdio_console_resume,
};

static __init void sandbox_stdio_console_init(void)
{
	sandbox_console_init();

	if(register_console(&stdio))
		LOG("Register console '%s'", stdio.name);
	else
		LOG("Failed to register console '%s'", stdio.name);
}

static __exit void sandbox_stdio_console_exit(void)
{
	sandbox_console_exit();

	if(unregister_console(&stdio))
		LOG("Unregister console '%s'", stdio.name);
	else
		LOG("Failed to unregister console '%s'", stdio.name);
}

device_initcall(sandbox_stdio_console_init);
device_exitcall(sandbox_stdio_console_exit);
