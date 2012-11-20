#include <xboot.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static int sample_add (lua_State * L)
{
	double a1 = luaL_checknumber(L, 1);
	double a2 = luaL_checknumber(L, 2);

	lua_pushnumber(L, a1 + a2);
	return 1;
}

static int sample_sub (lua_State * L)
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

int luaopen_xboot_sample (lua_State * L)
{
	luaL_newlib (L, sample_lib);
	return 1;
}
