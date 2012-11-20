#include <xboot.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

int luaopen_xboot_boot(lua_State * L)
{
	luaL_dofile(L, "/romdisk/boot.lua");
	return 1;
}
