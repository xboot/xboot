/*
 * framework/hardware/l-compass.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <compass/compass.h>
#include <framework/hardware/l-hardware.h>

static int l_compass_new(lua_State * L)
{
	const char * name = luaL_optstring(L, 1, NULL);
	struct compass_t * compass = name ? search_compass(name) : search_first_compass();
	if(!compass)
		return 0;
	lua_pushlightuserdata(L, compass);
	luaL_setmetatable(L, MT_HARDWARE_COMPASS);
	return 1;
}

static int l_compass_list(lua_State * L)
{
	struct device_t * pos, * n;
	struct compass_t * compass;

	lua_newtable(L);
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_COMPASS], head)
	{
		compass = (struct compass_t *)(pos->priv);
		if(!compass)
			continue;
		lua_pushlightuserdata(L, compass);
		luaL_setmetatable(L, MT_HARDWARE_COMPASS);
		lua_setfield(L, -2, pos->name);
	}
	return 1;
}

static const luaL_Reg l_compass[] = {
	{"new",		l_compass_new},
	{"list",	l_compass_list},
	{NULL, NULL}
};

static int m_compass_tostring(lua_State * L)
{
	struct compass_t * compass = luaL_checkudata(L, 1, MT_HARDWARE_COMPASS);
	lua_pushstring(L, compass->name);
	return 1;
}

static int m_compass_set_offset(lua_State * L)
{
	struct compass_t * compass = luaL_checkudata(L, 1, MT_HARDWARE_COMPASS);
	int ox = luaL_optinteger(L, 2, 0);
	int oy = luaL_optinteger(L, 3, 0);
	int oz = luaL_optinteger(L, 4, 0);
	compass_set_offset(compass, ox, oy, oz);
	lua_settop(L, 1);
	return 1;
}

static int m_compass_get_offset(lua_State * L)
{
	struct compass_t * compass = luaL_checkudata(L, 1, MT_HARDWARE_COMPASS);
	int ox, oy, oz;
	if(compass_get_offset(compass, &ox, &oy, &oz))
	{
		lua_pushinteger(L, (lua_Integer)ox);
		lua_pushinteger(L, (lua_Integer)oy);
		lua_pushinteger(L, (lua_Integer)oz);
		return 3;
	}
	return 0;
}

static int m_compass_get_magnetic(lua_State * L)
{
	struct compass_t * compass = luaL_checkudata(L, 1, MT_HARDWARE_COMPASS);
	int x, y, z;
	if(compass_get_magnetic(compass, &x, &y, &z))
	{
		lua_pushnumber(L, (lua_Number)x / 1000000);
		lua_pushnumber(L, (lua_Number)y / 1000000);
		lua_pushnumber(L, (lua_Number)z / 1000000);
		return 3;
	}
	return 0;
}

static int m_compass_get_heading(lua_State * L)
{
	struct compass_t * compass = luaL_checkudata(L, 1, MT_HARDWARE_COMPASS);
	int declination = luaL_optinteger(L, 2, 0);
	int heading;
	if(compass_get_heading(compass, &heading, declination))
	{
		lua_pushnumber(L, (lua_Number)heading);
		return 1;
	}
	return 0;
}

static const luaL_Reg m_compass[] = {
	{"__tostring",		m_compass_tostring},
	{"setOffset",		m_compass_set_offset},
	{"getOffset",		m_compass_get_offset},
	{"getMagnetic",		m_compass_get_magnetic},
	{"getHeading",		m_compass_get_heading},
	{NULL, NULL}
};

int luaopen_hardware_compass(lua_State * L)
{
	luaL_newlib(L, l_compass);
	luahelper_create_metatable(L, MT_HARDWARE_COMPASS, m_compass);
	return 1;
}
