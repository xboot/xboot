/*
 * framework/luahelper.c
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

#include <framework/luahelper.h>

void luahelper_dump_stack(lua_State * L)
{
	int top = lua_gettop(L);
	int i;

	printf("total in stack: %d\r\n", top);
	for(i = top; i >= 1; i--)
	{
		int t = lua_type(L, i);
		switch(t)
		{
		case LUA_TBOOLEAN:
			printf("[%d]%s: %s", i, lua_typename(L, t), lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TLIGHTUSERDATA:
			printf("[%d]%s: %p", i, lua_typename(L, t), lua_touserdata(L, i));
			break;
		case LUA_TNUMBER:
			printf("[%d]%s: %g", i, lua_typename(L, t), lua_tonumber(L, i));
			break;
		case LUA_TSTRING:
			printf("[%d]%s: %s", i, lua_typename(L, t), lua_tostring(L, i));
			break;
		case LUA_TTABLE:
			printf("[%d]%s", i, lua_typename(L, t));
			break;
		case LUA_TFUNCTION:
			printf("[%d]%s: %p", i, lua_typename(L, t), lua_tocfunction(L, i));
			break;
		case LUA_TUSERDATA:
			printf("[%d]%s: %p", i, lua_typename(L, t), lua_touserdata(L, i));
			break;
		case LUA_TTHREAD:
			printf("[%d]%s", i, lua_typename(L, t));
			break;
		default:
			printf("[%d]%s", i, lua_typename(L, t));
			break;
		}
		printf("\r\n");
	}
}

const char * luahelper_get_strfield(lua_State * L, const char * key, const char * def)
{
	const char * value;

	lua_getfield(L, -1, key);
	value = luaL_optstring(L, -1, def);
	lua_pop(L, 1);

	return value;
}

lua_Number luahelper_get_numfield(lua_State * L, const char * key, lua_Number def)
{
	lua_Number value;

	lua_getfield(L, -1, key);
	value = luaL_optnumber(L, -1, def);
	lua_pop(L, 1);

	return value;
}

lua_Integer luahelper_get_intfield(lua_State * L, const char * key, lua_Integer def)
{
	lua_Integer value;

	lua_getfield(L, -1, key);
	value = luaL_optinteger(L, -1, def);
	lua_pop(L, 1);

	return value;
}

void luahelper_set_strfield(lua_State * L, const char * key, const char * value)
{
	lua_pushstring(L, value);
	lua_setfield(L, -2, key);
}

void luahelper_set_numfield(lua_State * L, const char * key, lua_Number value)
{
	lua_pushnumber(L, value);
	lua_setfield(L, -2, key);
}

void luahelper_set_intfield(lua_State * L, const char * key, lua_Integer value)
{
	lua_pushinteger(L, value);
	lua_setfield(L, -2, key);
}

void luahelper_package_searcher(lua_State * L, lua_CFunction f, int pos)
{
	lua_getglobal(L, "table");
	lua_getfield(L, -1, "insert");
	lua_remove(L, -2);
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "searchers");
	lua_remove(L, -2);
	lua_pushvalue(L, -2);
	lua_pushvalue(L, -2);
	lua_pushnumber(L, pos);
	lua_pushcfunction(L, f);
	lua_call(L, 3, 0);
	lua_pop(L, 2);
}

void luahelper_package_path(lua_State * L, const char * path)
{
	lua_getglobal(L, "package");
	lua_pushstring(L, path);
	lua_setfield(L, -2, "path");
	lua_pop(L, 1);
}

void luahelper_package_cpath(lua_State * L, const char * cpath)
{
	lua_getglobal(L, "package");
	lua_pushstring(L, cpath);
	lua_setfield(L, -2, "cpath");
	lua_pop(L, 1);
}

void luahelper_preload(lua_State * L, const char * name, lua_CFunction f)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	lua_pushcfunction(L, f);
	lua_setfield(L, -2, name);
	lua_pop(L, 2);
}

void luahelper_create_metatable(lua_State * L, const char * name, const luaL_Reg * funcs)
{
	luaL_newmetatable(L, name);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, funcs, 0);
	lua_pop(L, 1);
}

static int msghandler(lua_State * L)
{
	const char * msg = lua_tostring(L, 1);
	if(msg == NULL)
	{
		if(luaL_callmeta(L, 1, "__tostring") && lua_type(L, -1) == LUA_TSTRING)
			return 1;
		else
			msg = lua_pushfstring(L, "(error object is a %s value)", luaL_typename(L, 1));
	}
	luaL_traceback(L, L, msg, 1);
	return 1;
}

int luahelper_pcall(lua_State * L, int narg, int nres)
{
	int status;
	int base = lua_gettop(L) - narg;
	lua_pushcfunction(L, msghandler);
	lua_insert(L, base);
	status = lua_pcall(L, narg, nres, base);
	lua_remove(L, base);
	return status;
}
