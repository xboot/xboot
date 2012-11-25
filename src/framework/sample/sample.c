/*
 * framework/sample.c
 *
 * Copyright (c) 2007-2012  jianjun jiang <jerryjianjun@gmail.com>
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

#include <framework/framework.h>

static int sample_add (lua_State * L)
{
	double a1 = luaL_checknumber(L, 1);
	double a2 = luaL_checknumber(L, 2);

	lua_pushnumber(L, a1 + a2);
	return 1;
}

static int sample_sub(lua_State * L)
{
	double a1 = luaL_checknumber(L, 1);
	double a2 = luaL_checknumber(L, 2);

	lua_pushnumber(L, a1 - a2);
	return 1;
}

static const luaL_Reg sample_lib[] = {
	{ "add", sample_add },
	{ "sub", sample_sub },
	{ NULL, NULL }
};

int luaopen_sample(lua_State * L)
{
	luaL_newlib (L, sample_lib);
	return 1;
}
