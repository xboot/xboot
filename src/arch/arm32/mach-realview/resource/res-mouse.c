/*
 * resource/res-mouse.c
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
#include <realview-mouse.h>

static struct realview_mouse_data_t realview_mouse_data = {
	.regbase	= REALVIEW_MOUSE_BASE,
	.width		= 640,
	.height		= 480,
};

static struct resource_t res_mouse = {
	.name		= "mouse-pl050",
	.id			= -1,
	.data		= &realview_mouse_data,
};

static __init void resource_mouse_init(void)
{
	if(register_resource(&res_mouse))
		LOG("Register resource '%s.%d'", res_mouse.name, res_mouse.id);
	else
		LOG("Failed to register resource '%s.%d'", res_mouse.name, res_mouse.id);
}

static __exit void resource_mouse_exit(void)
{
	if(unregister_resource(&res_mouse))
		LOG("Unregister resource '%s.%d'", res_mouse.name, res_mouse.id);
	else
		LOG("Failed to unregister resource '%s.%d'", res_mouse.name, res_mouse.id);
}

resource_initcall(resource_mouse_init);
resource_exitcall(resource_mouse_exit);
