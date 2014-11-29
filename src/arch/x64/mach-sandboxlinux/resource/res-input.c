/*
 * resource/res-input.c
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
#include <sandboxlinux-input.h>

static struct sandboxlinux_input_data_t input_datas[] = {
	[0] = {
		.type	= INPUT_TYPE_KEYBOARD,
	},
	[1] = {
		.type	= INPUT_TYPE_MOUSE,
	},
	[2] = {
		.type	= INPUT_TYPE_TOUCHSCREEN,
	},
	[4] = {
		.type	= INPUT_TYPE_JOYSTICK,
	},
};

static struct resource_t res_inputs[] = {
	{
		.name	= "sandboxlinux-input",
		.id		= 0,
		.data	= &input_datas[0],
	}, {
		.name	= "sandboxlinux-input",
		.id		= 1,
		.data	= &input_datas[1],
	}, {
		.name	= "sandboxlinux-input",
		.id		= 2,
		.data	= &input_datas[2],
	}, {
		.name	= "sandboxlinux-input",
		.id		= 3,
		.data	= &input_datas[3],
	},
};

static __init void resource_input_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_inputs); i++)
	{
		if(register_resource(&res_inputs[i]))
			LOG("Register resource %s:'%s.%d'", res_inputs[i].mach, res_inputs[i].name, res_inputs[i].id);
		else
			LOG("Failed to register resource %s:'%s.%d'", res_inputs[i].mach, res_inputs[i].name, res_inputs[i].id);
	}
}

static __exit void resource_input_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(res_inputs); i++)
	{
		if(unregister_resource(&res_inputs[i]))
			LOG("Unregister resource %s:'%s.%d'", res_inputs[i].mach, res_inputs[i].name, res_inputs[i].id);
		else
			LOG("Failed to unregister resource %s:'%s.%d'", res_inputs[i].mach, res_inputs[i].name, res_inputs[i].id);
	}
}

resource_initcall(resource_input_init);
resource_exitcall(resource_input_exit);
