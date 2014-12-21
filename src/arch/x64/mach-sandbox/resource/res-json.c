/*
 * resource/res-json.c
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
#include <json.h>
#include <sandbox.h>
#include <console/console.h>
#include <sandbox-input.h>

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
	char * in = NULL, * out = NULL, * err = NULL;
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

		if(in && out && err)
		{
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

static void json_resource_led(json_value * data)
{
}

static __init void resource_json_init(void)
{
	struct sandbox_config_t * cfg = sandbox_get_config();
	json_value * value = json_parse(cfg->json, strlen(cfg->json));
	int i;

	if(value->type == json_object)
	{
		for(i = 0; i < value->u.object.length; i++)
		{
			if(strcmp(value->u.object.values[i].name, "console") == 0)
			{
				json_resource_console(value->u.object.values[i].value);
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
