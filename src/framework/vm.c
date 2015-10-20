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

static int l_logger_print(lua_State * L)
{
	int n = lua_gettop(L);
	int i;

	lua_getglobal(L, "tostring");
	for(i = 1; i <= n; i++)
	{
		const char * s;
		size_t l;
		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		s = lua_tolstring(L, -1, &l);
		if(s == NULL)
			return luaL_error(L, "'tostring' must return a string to 'print'");
		if(i > 1)
			logger_output("\t", 1);
		logger_output(s, l);
		lua_pop(L, 1);
	}
	logger_output("\r\n", 2);

	return 0;
}

static int pmain(lua_State * L)
{
	int argc = (int)lua_tointeger(L, 1);
	char ** argv = (char **)lua_touserdata(L, 2);
	int i;

	/* open default libs */
	luaL_openlibs(L);

	/* override print */
	lua_pushcfunction(L, l_logger_print);
	lua_pushvalue(L, -1);
	lua_setglobal(L, "print");

	/* create arg table */
	lua_createtable(L, argc, 0);
	for(i = 0; i < argc; i++)
	{
		lua_pushstring(L, argv[i]);
		lua_rawseti(L, -2, i);
	}
	lua_setglobal(L, "arg");

	/* preload xboot */
	luahelper_preload(L, "xboot", luaopen_xboot);

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
	return 1;
}

int vm_exec(const char * path, int argc, char ** argv)
{
	struct runtime_t rt, *r;
	lua_State * L;
	int status = LUA_ERRERR, result;

	runtime_create_save(&rt, 0, 0, path, &r);
	L = luaL_newstate();
	if(L)
	{
		lua_pushcfunction(L, &pmain);
		lua_pushinteger(L, argc);
		lua_pushlightuserdata(L, argv);
		status = lua_pcall(L, 2, 1, 0);
		result = lua_toboolean(L, -1);
		if(status != LUA_OK)
		{
			const char * msg = lua_tostring(L, -1);
			lua_writestringerror("%s: ", argv[0]);
			lua_writestringerror("%s\n", msg);
			lua_pop(L, 1);
		}
		lua_close(L);
	}
	runtime_destroy_restore(&rt, r);
	return (result && (status == LUA_OK)) ? 0 : -1;
}
