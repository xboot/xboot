/*
 * resource/res-console.c
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

static struct console_stdio_data_t console_stdio_data = {
	.in		= "exynos4412-uart.3",
	.out	= "exynos4412-uart.3",
	.err	= "exynos4412-uart.3",
};

static struct resource_t res_console = {
	.name	= "console",
	.id		= -1,
	.data	= &console_stdio_data,
};

static __init void resource_console_init(void)
{
	if(register_resource(&res_console))
		LOG("Register resource '%s.%d'", res_console.name, res_console.id);
	else
		LOG("Failed to register resource '%s.%d'", res_console.name, res_console.id);
}

static __exit void resource_console_exit(void)
{
	if(unregister_resource(&res_console))
		LOG("Unregister resource '%s.%d'", res_console.name, res_console.id);
	else
		LOG("Failed to unregister resource '%s.%d'", res_console.name, res_console.id);
}

resource_initcall(resource_console_init);
resource_exitcall(resource_console_exit);
