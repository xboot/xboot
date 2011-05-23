#ifndef __LUALIB_H__
#define __LUALIB_H__

#include <lua/luacfg.h>
#include <lua/llimits.h>
#include <lua/lua.h>

#define LUA_COLIBNAME	"coroutine"
int luaopen_base(lua_State *L);

#define LUA_TABLIBNAME	"table"
int luaopen_table(lua_State *L);

#define LUA_IOLIBNAME	"io"
int luaopen_io(lua_State *L);

#define LUA_OSLIBNAME	"os"
int luaopen_os(lua_State *L);

#define LUA_STRLIBNAME	"string"
int luaopen_string(lua_State *L);

#define LUA_MATHLIBNAME	"math"
int luaopen_math(lua_State *L);

#define LUA_DBLIBNAME	"debug"
int luaopen_debug(lua_State *L);

#define LUA_LOADLIBNAME	"package"
int luaopen_package(lua_State *L);

/*
 * open all previous libraries
 */
void luaL_openlibs(lua_State * L);

#endif /* __LUALIB_H__ */
