/*
 * kernel/core/dtree.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
	struct json_value_t * v;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_BOOLEAN))
					return v->u.boolean ? 1 : 0;
			}
		}
	}
	return def;
}

int dt_read_int(struct dtnode_t * n, const char * name, int def)
{
	struct json_value_t * v;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_INTEGER))
					return (int)v->u.integer;
			}
		}
	}
	return def;
}

long long dt_read_long(struct dtnode_t * n, const char * name, long long def)
{
	struct json_value_t * v;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_INTEGER))
					return (long long)v->u.integer;
			}
		}
	}
	return def;
}

double dt_read_double(struct dtnode_t * n, const char * name, double def)
{
	struct json_value_t * v;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_DOUBLE))
					return (double)v->u.dbl;
			}
		}
	}
	return def;
}

char * dt_read_string(struct dtnode_t * n, const char * name, char * def)
{
	struct json_value_t * v;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_STRING))
					return (char *)v->u.string.ptr;
			}
		}
	}
	return def;
}

u8_t dt_read_u8(struct dtnode_t * n, const char * name, u8_t def)
{
	struct json_value_t * v;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_INTEGER))
					return (u8_t)v->u.integer;
			}
		}
	}
	return def;
}

u16_t dt_read_u16(struct dtnode_t * n, const char * name, u16_t def)
{
	struct json_value_t * v;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_INTEGER))
					return (u16_t)v->u.integer;
			}
		}
	}
	return def;
}

u32_t dt_read_u32(struct dtnode_t * n, const char * name, u32_t def)
{
	struct json_value_t * v;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_INTEGER))
					return (u32_t)v->u.integer;
			}
		}
	}
	return def;
}

u64_t dt_read_u64(struct dtnode_t * n, const char * name, u64_t def)
{
	struct json_value_t * v;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_INTEGER))
					return (u64_t)v->u.integer;
			}
		}
	}
	return def;
}

struct dtnode_t * dt_read_object(struct dtnode_t * n, const char * name, struct dtnode_t * o)
{
	struct json_value_t * v;
	int i;

	if(o && n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_OBJECT))
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
	struct json_value_t * v;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_ARRAY))
					return v->u.array.length;
			}
		}
	}
	return 0;
}

int dt_read_array_bool(struct dtnode_t * n, const char * name, int idx, int def)
{
	struct json_value_t * v, * e;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_ARRAY))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == JSON_BOOLEAN))
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
	struct json_value_t * v, * e;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_ARRAY))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == JSON_INTEGER))
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
	struct json_value_t * v, * e;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_ARRAY))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == JSON_INTEGER))
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
	struct json_value_t * v, * e;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_ARRAY))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == JSON_DOUBLE))
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
	struct json_value_t * v, * e;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_ARRAY))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == JSON_STRING))
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
	struct json_value_t * v, * e;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_ARRAY))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == JSON_INTEGER))
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
	struct json_value_t * v, * e;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_ARRAY))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == JSON_INTEGER))
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
	struct json_value_t * v, * e;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_ARRAY))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == JSON_INTEGER))
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
	struct json_value_t * v, * e;
	int i;

	if(n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_ARRAY))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == JSON_INTEGER))
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
	struct json_value_t * v, * e;
	int i;

	if(o && n && n->value && (n->value->type == JSON_OBJECT))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) == 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == JSON_ARRAY))
				{
					if(idx >= 0 && (idx < v->u.array.length))
					{
						e = v->u.array.values[idx];
						if(e && (e->type == JSON_OBJECT))
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
