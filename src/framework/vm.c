/*
 * framework/vm.c
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

#include <framework/framework.h>
#include <framework/vm.h>

int vm_exec(const char * path, int argc, char * argv[])
{
	struct runtime_t rt, *r;
	lua_State * L;
	int i, ret = -1;

	runtime_create_save(&rt, 0, 0, path, &r);
	L = luaL_newstate();

	luaL_openlibs(L);
	luahelper_preload(L, "xboot", luaopen_xboot);
	do {
		lua_newtable(L);
		if(argc > 0)
		{
			lua_pushstring(L, argv[0]);
			lua_rawseti(L, -2, -2);
		}
		lua_pushstring(L, "embedded boot.lua");
		lua_rawseti(L, -2, -1);
		for(i = 1; i < argc; i++)
		{
			lua_pushstring(L, argv[i]);
			lua_rawseti(L, -2, i);
		}
		lua_setglobal(L, "arg");
	} while(0);

	/* require xboot */
	lua_getglobal(L, "require");
	lua_pushstring(L, "xboot");
	lua_call(L, 1, 1);
	lua_pop(L, 1);

	/* require xboot.boot */
	lua_getglobal(L, "require");
	lua_pushstring(L, "xboot.boot");
	lua_call(L, 1, 1);

	/* call xboot.boot */
	lua_call(L, 0, 1);
	if(lua_isnumber(L, -1))
		ret = (int)lua_tonumber(L, -1);

	lua_close(L);
	runtime_destroy_restore(&rt, r);
	return ret;
}
