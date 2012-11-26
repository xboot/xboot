/*
 * framework/framework.c
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

extern int luaopen_framerate(lua_State * L);
extern int luaopen_sample(lua_State * L);

static bool_t vm_preload(lua_State * L, const char * name, lua_CFunction f)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	lua_pushcfunction(L, f);
	lua_setfield(L, -2, name);
	lua_pop(L, 2);

	return TRUE;
}

static const luaL_Reg xboot_libs[] = {
	{ "xboot.framerate", luaopen_framerate },
	{ "xboot.sample", luaopen_sample },
	{ "xboot.boot", luaopen_boot },
	{ NULL, NULL }
};

int luaopen_xboot(lua_State * L)
{
	int i;

	lua_getglobal(L, "xboot");
	if(!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		lua_setglobal(L, "xboot");
	}

	lua_pushstring(L, XBOOT_VERSION);
	lua_setfield(L, -2, "_VERSION");

	lua_pushnumber(L, XBOOT_MAJOY);
	lua_setfield(L, -2, "_MAJOY");

	lua_pushnumber(L, XBOOT_MINIOR);
	lua_setfield(L, -2, "_MINIOR");

	lua_pushnumber(L, XBOOT_PATCH);
	lua_setfield(L, -2, "_PATCH");

	lua_pushstring(L, __ARCH__);
	lua_setfield(L, -2, "_ARCH");

	lua_pushstring(L, __MACH__);
	lua_setfield(L, -2, "_MACH");

	vm_preload(L, "xboot", luaopen_xboot);
	for(i = 0; xboot_libs[i].name != 0; i++)
	{
		vm_preload(L, xboot_libs[i].name, xboot_libs[i].func);
	}

	return 1;
}
