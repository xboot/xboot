/*
 * resource/res-json.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
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
#include <json.h>
#include <sandbox.h>
#include <console/console.h>
#include <sandbox-fb.h>
#include <sandbox-input.h>
#include <sandbox-led.h>

static void json_resource_register(struct resource_t * res)
{
	if(register_resource(res))
		LOG("Register resource %s:'%s.%d'", res->mach, res->name, res->id);
	else
		LOG("Failed to register resource %s:'%s.%d'", res->mach, res->name, res->id);
}

static void json_resource_console(json_value * value)
{
	struct resource_t * res;
	struct console_stdio_data_t * data;
	char * in = "stdio", * out = "stdio", * err = "stdio";
	json_value * v;
	int i;

	if(value->type == json_object)
	{
		for(i = 0; i < value->u.object.length; i++)
		{
			if(strcmp(value->u.object.values[i].name, "in") == 0)
			{
				v = value->u.object.values[i].value;
				if(v->type == json_string)
					in = v->u.string.ptr;
			}
			else if(strcmp(value->u.object.values[i].name, "out") == 0)
			{
				v = value->u.object.values[i].value;
				if(v->type == json_string)
					out = v->u.string.ptr;
			}
			else if(strcmp(value->u.object.values[i].name, "err") == 0)
			{
				v = value->u.object.values[i].value;
				if(v->type == json_string)
					err = v->u.string.ptr;
			}
		}

		res = malloc(sizeof(struct resource_t));
		data = malloc(sizeof(struct console_stdio_data_t));
		data->in = strdup(in);
		data->out = strdup(out);
		data->err = strdup(err);
		res->mach = NULL;
		res->name = "console";
		res->id = -1;
		res->data = data;
		json_resource_register(res);
	}
}

static void json_resource_framebuffer(json_value * value)
{
	struct resource_t * res;
	struct sandbox_fb_data_t * data;
	int width = 640, height = 480, xdpi = 160, ydpi = 160, fullscreen = 0;
	json_value * v;
	int i;

	if(value->type == json_object)
	{
		for(i = 0; i < value->u.object.length; i++)
		{
			if(strcmp(value->u.object.values[i].name, "width") == 0)
			{
				v = value->u.object.values[i].value;
				if(v->type == json_integer)
					width = v->u.integer;
			}
			else if(strcmp(value->u.object.values[i].name, "height") == 0)
			{
				v = value->u.object.values[i].value;
				if(v->type == json_integer)
					height = v->u.integer;
			}
			else if(strcmp(value->u.object.values[i].name, "xdpi") == 0)
			{
				v = value->u.object.values[i].value;
				if(v->type == json_integer)
					xdpi = v->u.integer;
			}
			else if(strcmp(value->u.object.values[i].name, "ydpi") == 0)
			{
				v = value->u.object.values[i].value;
				if(v->type == json_integer)
					ydpi = v->u.integer;
			}
			else if(strcmp(value->u.object.values[i].name, "fullscreen") == 0)
			{
				v = value->u.object.values[i].value;
				if(v->type == json_boolean)
					fullscreen = (v->u.boolean != 0) ? 1 : 0;
			}
		}

		res = malloc(sizeof(struct resource_t));
		data = malloc(sizeof(struct sandbox_fb_data_t));
		data->width = width;
		data->height = height;
		data->xdpi = xdpi;
		data->ydpi = ydpi;
		data->fullscreen = fullscreen;
		data->priv = NULL;
		res->mach = NULL;
		res->name = "sandbox-fb";
		res->id = -1;
		res->data = data;
		json_resource_register(res);
	}
}

static void json_resource_input(json_value * value)
{
	struct resource_t * res;
	struct sandbox_input_data_t * data;
	enum input_type_t type;
	json_value * v;
	int i;

	if(value->type == json_array)
	{
		for(i = 0; i < value->u.array.length; i++)
		{
			v = value->u.array.values[i];
			if(v->type == json_string)
			{
				if(strcmp(v->u.string.ptr, "keyboard") == 0)
					type = INPUT_TYPE_KEYBOARD;
				else if(strcmp(v->u.string.ptr, "mouse") == 0)
					type = INPUT_TYPE_MOUSE;
				else if(strcmp(v->u.string.ptr, "touchscreen") == 0)
					type = INPUT_TYPE_TOUCHSCREEN;
				else if(strcmp(v->u.string.ptr, "joystick") == 0)
					type = INPUT_TYPE_JOYSTICK;
				else
					type = -1;

				if(type != -1)
				{
					res = malloc(sizeof(struct resource_t));
					data = malloc(sizeof(struct sandbox_input_data_t));
					data->type = type;
					res->mach = NULL;
					res->name = "sandbox-input";
					res->id = -1;
					res->data = data;
					json_resource_register(res);
				}
			}
		}
	}
}

static void json_resource_led(json_value * value)
{
	struct resource_t * res;
	struct sandbox_led_data_t * data;
	json_value * v;
	int i;

	if(value->type == json_array)
	{
		for(i = 0; i < value->u.array.length; i++)
		{
			v = value->u.array.values[i];
			if(v->type == json_string)
			{
				if(sandbox_sysfs_access(v->u.string.ptr, "rw") == 0)
				{
					res = malloc(sizeof(struct resource_t));
					data = malloc(sizeof(struct sandbox_led_data_t));
					data->path = strdup(v->u.string.ptr);
					res->mach = NULL;
					res->name = "sandbox-led";
					res->id = -1;
					res->data = data;
					json_resource_register(res);
				}
			}
		}
	}
}

static __init void resource_json_init(void)
{
	struct sandbox_t * sandbox = sandbox_get();
	json_value * value = json_parse(sandbox->config.buffer, sandbox->config.size);
	int i;

	if(value && (value->type == json_object))
	{
		for(i = 0; i < value->u.object.length; i++)
		{
			if(strcmp(value->u.object.values[i].name, "console") == 0)
			{
				json_resource_console(value->u.object.values[i].value);
			}
			else if(strcmp(value->u.object.values[i].name, "framebuffer") == 0)
			{
				json_resource_framebuffer(value->u.object.values[i].value);
			}
			else if(strcmp(value->u.object.values[i].name, "input") == 0)
			{
				json_resource_input(value->u.object.values[i].value);
			}
			else if(strcmp(value->u.object.values[i].name, "led") == 0)
			{
				json_resource_led(value->u.object.values[i].value);
			}
		}
	}
	json_value_free(value);
}

static __exit void resource_json_exit(void)
{
}

resource_initcall(resource_json_init);
resource_exitcall(resource_json_exit);
