/*
 * framework/vm.c
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

#include <framework/framework.h>
#include <framework/vm.h>

static int luaopen_boot(lua_State * L)
{
	return (luaL_dofile(L, "/romdisk/framework/org/xboot/boot.lua") == LUA_OK) ? 1 : 0;
}

int vm_exec(const char * path, int argc, char * argv[])
{
	struct runtime_t rt, *r;
	lua_State * lua;
	int i, ret = -1;

	runtime_create_save(&rt, 0, 0, path, &r);
	lua = luaL_newstate();
	luaL_openlibs(lua);
	luaopen_xboot(lua);
	do {
		lua_newtable(lua);
		if(argc > 0)
		{
			lua_pushstring(lua, argv[0]);
			lua_rawseti(lua, -2, -2);
		}
		lua_pushstring(lua, "embedded boot.lua");
		lua_rawseti(lua, -2, -1);
		for(i = 1; i < argc; i++)
		{
			lua_pushstring(lua, argv[i]);
			lua_rawseti(lua, -2, i);
		}
		lua_setglobal(lua, "arg");
	} while(0);

	luaopen_boot(lua);
	lua_call(lua, 0, 1);
	if(lua_isnumber(lua, 1))
		ret = (int)lua_tonumber(lua, 1);
	lua_pop(lua, 1);

	lua_close(lua);
	runtime_destroy_restore(&rt, r);
	return ret;
}
