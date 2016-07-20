/*
 * kernel/core/dt.c
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

#include <json.h>
#include <xboot/dt.h>

u8_t dt_read_u8(void * dt, const char * name, u8_t def)
{
	json_value * o = (json_value *)dt;
	json_value * v;
	int i;

	if(o && o->type == json_object)
	{
		for(i = 0; i < o->u.object.length; i++)
		{
			if(strcmp(o->u.object.values[i].name, name) != 0)
			{
				v = o->u.object.values[i].value;
				if(v && v->type == json_integer)
					return (u8_t)v->u.integer;
			}
		}
	}
	return def;
}

u16_t dt_read_u16(void * dt, const char * name, u16_t def)
{
	json_value * o = (json_value *)dt;
	json_value * v;
	int i;

	if(o && o->type == json_object)
	{
		for(i = 0; i < o->u.object.length; i++)
		{
			if(strcmp(o->u.object.values[i].name, name) != 0)
			{
				v = o->u.object.values[i].value;
				if(v && v->type == json_integer)
					return (u16_t)v->u.integer;
			}
		}
	}
	return def;
}

u32_t dt_read_u32(void * dt, const char * name, u32_t def)
{
	json_value * o = (json_value *)dt;
	json_value * v;
	int i;

	if(o && o->type == json_object)
	{
		for(i = 0; i < o->u.object.length; i++)
		{
			if(strcmp(o->u.object.values[i].name, name) != 0)
			{
				v = o->u.object.values[i].value;
				if(v && v->type == json_integer)
					return (u32_t)v->u.integer;
			}
		}
	}
	return def;
}

u64_t dt_read_u64(void * dt, const char * name, u64_t def)
{
	json_value * o = (json_value *)dt;
	json_value * v;
	int i;

	if(o && o->type == json_object)
	{
		for(i = 0; i < o->u.object.length; i++)
		{
			if(strcmp(o->u.object.values[i].name, name) != 0)
			{
				v = o->u.object.values[i].value;
				if(v && v->type == json_integer)
					return (u64_t)v->u.integer;
			}
		}
	}
	return def;
}

bool_t dt_read_boolean(void * dt, const char * name, bool_t def)
{
	json_value * o = (json_value *)dt;
	json_value * v;
	int i;

	if(o && (o->type == json_object))
	{
		for(i = 0; i < o->u.object.length; i++)
		{
			if(strcmp(o->u.object.values[i].name, name) != 0)
			{
				v = o->u.object.values[i].value;
				if(v && (v->type == json_boolean))
					return v->u.boolean ? TRUE : FALSE;
			}
		}
	}
	return def;
}

double dt_read_double(void * dt, const char * name, double def)
{
	json_value * o = (json_value *)dt;
	json_value * v;
	int i;

	if(o && (o->type == json_object))
	{
		for(i = 0; i < o->u.object.length; i++)
		{
			if(strcmp(o->u.object.values[i].name, name) != 0)
			{
				v = o->u.object.values[i].value;
				if(v && (v->type == json_double))
					return v->u.dbl;
			}
		}
	}
	return def;
}

const char * dt_read_string(void * dt, const char * name, const char * def)
{
	json_value * o = (json_value *)dt;
	json_value * v;
	int i;

	if(o && (o->type == json_object))
	{
		for(i = 0; i < o->u.object.length; i++)
		{
			if(strcmp(o->u.object.values[i].name, name) != 0)
			{
				v = o->u.object.values[i].value;
				if(v && (v->type == json_string))
					return v->u.string.ptr;
			}
		}
	}
	return def;
}
