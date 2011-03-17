/*
 * resource/res-keypad.c
 *
 * Copyright (c) 2007-2010  jianjun jiang <jerryjianjun@gmail.com>
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

#include <configs.h>
#include <default.h>
#include <types.h>
#include <xboot/log.h>
#include <xboot/initcall.h>
#include <xboot/resource.h>
#include <input/keyboard/keyboard.h>
#include <s5pv210-keypad.h>

static struct s5pv210_keypad keypad_data = {
	.rows	= 2,
	.cols	= 2,
	.keycode = {
		KEY_DOWN,	KEY_ENTER,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0, 0,	0,
		KEY_ENTER,	0,			0,	0,	0,	0,	0,	0,	0,	0,	0,	0, 0,	0,
		0,			0,			0,	0,	0,	0,	0,	0,	0,	0,	0,	0, 0,	0,
		0,			0,			0,	0,	0,	0,	0,	0,	0,	0,	0,	0, 0,	0,
		0,			0,			0,	0,	0,	0,	0,	0,	0,	0,	0,	0, 0,	0,
		0,			0,			0,	0,	0,	0,	0,	0,	0,	0,	0,	0, 0,	0,
		0,			0,			0,	0,	0,	0,	0,	0,	0,	0,	0,	0, 0,	0,
		0,			0,			0,	0,	0,	0,	0,	0,	0,	0,	0,	0, 0,	0,
	},
};

static struct resource keypad_res = {
	.name		= "keypad",
	.data		= &keypad_data,
};

static __init void res_keypad_init(void)
{
	if(!register_resource(&keypad_res))
		LOG_E("failed to register resource '%s'", keypad_res.name);
}

static __exit void res_keypad_exit(void)
{
	if(!unregister_resource(&keypad_res))
		LOG_E("failed to unregister resource '%s'", keypad_res.name);
}

module_init(res_keypad_init, LEVEL_MACH_RES);
module_exit(res_keypad_exit, LEVEL_MACH_RES);
