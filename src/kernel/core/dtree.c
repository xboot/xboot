/*
 * kernel/core/dtree.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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
#include <xboot/dtree.h>

const char * dt_read_name(struct dtnode_t * n)
{
	return n ? n->name : NULL;
}

int dt_read_id(struct dtnode_t * n)
{
	return n ? (int)n->addr : 0;
}

physical_addr_t dt_read_address(struct dtnode_t * n)
{
	return n ? n->addr : 0;
}

int dt_read_bool(struct dtnode_t * n, const char * name, int def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_boolean))
					return v->u.boolean ? 1 : 0;
			}
		}
	}
	return def;
}

int dt_read_int(struct dtnode_t * n, const char * name, int def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_integer))
					return (int)v->u.integer;
			}
		}
	}
	return def;
}

long long dt_read_long(struct dtnode_t * n, const char * name, long long def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_integer))
					return (long long)v->u.integer;
			}
		}
	}
	return def;
}

double dt_read_double(struct dtnode_t * n, const char * name, double def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_double))
					return (double)v->u.dbl;
			}
		}
	}
	return def;
}

char * dt_read_string(struct dtnode_t * n, const char * name, char * def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_string))
					return (char *)v->u.string.ptr;
			}
		}
	}
	return def;
}

u8_t dt_read_u8(struct dtnode_t * n, const char * name, u8_t def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_integer))
					return (u8_t)v->u.integer;
			}
		}
	}
	return def;
}

u16_t dt_read_u16(struct dtnode_t * n, const char * name, u16_t def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_integer))
					return (u16_t)v->u.integer;
			}
		}
	}
	return def;
}

u32_t dt_read_u32(struct dtnode_t * n, const char * name, u32_t def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_integer))
					return (u32_t)v->u.integer;
			}
		}
	}
	return def;
}

u64_t dt_read_u64(struct dtnode_t * n, const char * name, u64_t def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_integer))
					return (u64_t)v->u.integer;
			}
		}
	}
	return def;
}

struct dtnode_t * dt_read_object(struct dtnode_t * n, const char * name, struct dtnode_t * o)
{
	json_value * v;
	int i;

	if(o && n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_object))
				{
					o->name = name;
					o->addr = 0;
					o->value = v;
					return o;
				}
			}
		}
	}
	return NULL;
}

int dt_read_array_length(struct dtnode_t * n, const char * name)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
					return v->u.array.length;
			}
		}
	}
	return 0;
}

int dt_read_array_bool(struct dtnode_t * n, const char * name, int idx, int def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_boolean))
							return e->u.boolean ? 1 : 0;
					}
				}
			}
		}
	}
	return def;
}

int dt_read_array_int(struct dtnode_t * n, const char * name, int idx, int def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_integer))
							return (int)e->u.integer;
					}
				}
			}
		}
	}
	return def;
}

long long dt_read_array_long(struct dtnode_t * n, const char * name, int idx, long long def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_integer))
							return (long long)e->u.integer;
					}
				}
			}
		}
	}
	return def;
}

double dt_read_array_double(struct dtnode_t * n, const char * name, int idx, double def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_double))
							return (double)e->u.dbl;
					}
				}
			}
		}
	}
	return def;
}

char * dt_read_array_string(struct dtnode_t * n, const char * name, int idx, char * def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_string))
							return (char *)e->u.string.ptr;
					}
				}
			}
		}
	}
	return def;
}

u8_t dt_read_array_u8(struct dtnode_t * n, const char * name, int idx, u8_t def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_integer))
							return (u8_t)e->u.integer;
					}
				}
			}
		}
	}
	return def;
}

u16_t dt_read_array_u16(struct dtnode_t * n, const char * name, int idx, u16_t def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_integer))
							return (u16_t)e->u.integer;
					}
				}
			}
		}
	}
	return def;
}

u32_t dt_read_array_u32(struct dtnode_t * n, const char * name, int idx, u32_t def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_integer))
							return (u32_t)e->u.integer;
					}
				}
			}
		}
	}
	return def;
}

u64_t dt_read_array_u64(struct dtnode_t * n, const char * name, int idx, u64_t def)
{
	json_value * v, * e;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_integer))
							return (u64_t)e->u.integer;
					}
				}
			}
		}
	}
	return def;
}

struct dtnode_t * dt_read_array_object(struct dtnode_t * n, const char * name, int idx, struct dtnode_t * o)
{
	json_value * v, * e;
	int i;

	if(o && n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_array))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == json_object))
						{
							o->name = 0;
							o->addr = 0;
							o->value = e;
							return o;
						}
					}
				}
			}
		}
	}
	return NULL;
}
