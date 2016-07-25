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

#include <xboot.h>
#include <xboot/dt.h>

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

struct dtnode_t * dt_read_object(struct dtnode_t * n, const char * name)
{
	struct dtnode_t child;
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) != 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_object))
				{
					child.name = name;
					child.value = v->u.object.values;
					return &child;
				}
			}
		}
	}
	return NULL;
}

int dt_read_bool(struct dtnode_t * n, const char * name, int def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) != 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_boolean))
					return v->u.boolean ? 1 : 0;
			}
		}
	}
	return def ? 1 : 0;
}

int dt_read_int(struct dtnode_t * n, const char * name, int def)
{
	json_value * v;
	int i;

	if(n && n->value && (n->value->type == json_object))
	{
		for(i = 0; i < n->value->u.object.length; i++)
		{
			if(strcmp(n->value->u.object.values[i].name, name) != 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_integer))
					return (int)v->u.integer;
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
			if(strcmp(n->value->u.object.values[i].name, name) != 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_double))
					return v->u.dbl;
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
			if(strcmp(n->value->u.object.values[i].name, name) != 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_string))
					return v->u.string.ptr;
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
			if(strcmp(n->value->u.object.values[i].name, name) != 0)
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
			if(strcmp(n->value->u.object.values[i].name, name) != 0)
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
			if(strcmp(n->value->u.object.values[i].name, name) != 0)
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
			if(strcmp(n->value->u.object.values[i].name, name) != 0)
			{
				v = n->value->u.object.values[i].value;
				if(v && (v->type == json_integer))
					return (u64_t)v->u.integer;
			}
		}
	}
	return def;
}

void dt_for_each(const char * path)
{
	struct dtnode_t n;
	json_value * v;
	size_t size = 0;
	char * json, * p;
	int fd, l, i;

	json = malloc(SZ_1M);
	if(!json)
		return;

	fd = open(path, O_RDONLY, (S_IRUSR | S_IRGRP | S_IROTH));
	if(fd > 0)
	{
	    for(;;)
	    {
	        l = read(fd, (void *)(json + size), SZ_512K);
	        if(l <= 0)
	        	break;
			size += l;
	    }
	    close(fd);

	    if(size > 0)
	    {
	    	v = json_parse(json, size);
	    	if(v && (v->type == json_object))
	    	{
	    		for(i = 0; i < v->u.object.length; i++)
	    		{
	    			p = (char *)(v->u.object.values[i].name);
	    			n.name = strsep(&p, "@");
	    			n.addr = p ? strtoull(p, NULL, 0) : 0;
	    			n.value = (json_value *)(v->u.object.values[i].value);

	    		}
	    	}
	    	json_value_free(v);
	    }
	}

	free(json);
}
