/*
 * lua/linit.c
 *
 * Initialization of libraries
 */

#include <lua/luacfg.h>
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

static const luaL_Reg lualibs[] = {
	{"", luaopen_base},
	{LUA_LOADLIBNAME, luaopen_package},
	{LUA_TABLIBNAME, luaopen_table},
	{LUA_IOLIBNAME, luaopen_io},
//TODO	{LUA_OSLIBNAME, luaopen_os},
	{LUA_STRLIBNAME, luaopen_string},
	{LUA_MATHLIBNAME, luaopen_math},
	{LUA_DBLIBNAME, luaopen_debug},
	{NULL, NULL}
};

void luaL_openlibs(lua_State * L)
{
	const luaL_Reg * lib = lualibs;

	for (; lib->func; lib++)
	{
		lua_pushcfunction(L, lib->func);
		lua_pushstring(L, lib->name);
		lua_call(L, 1, 0);
	}
}
