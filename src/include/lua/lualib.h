#ifndef __LUALIB_H__
#define __LUALIB_H__

#include <lua/luacfg.h>
#include <lua/llimits.h>
#include <lua/lua.h>

/* Key to file-handle type */
//xxx
#define LUA_FILEHANDLE		"FILE*"


#define LUA_COLIBNAME		"coroutine"
int luaopen_base(lua_State *L);

//xxx
#if 0

#define LUA_TABLIBNAME	"table"
LUALIB_API int (luaopen_table) (lua_State *L);

#define LUA_IOLIBNAME	"io"
LUALIB_API int (luaopen_io) (lua_State *L);

#define LUA_OSLIBNAME	"os"
LUALIB_API int (luaopen_os) (lua_State *L);

#define LUA_STRLIBNAME	"string"
LUALIB_API int (luaopen_string) (lua_State *L);

#define LUA_MATHLIBNAME	"math"
LUALIB_API int (luaopen_math) (lua_State *L);

#define LUA_DBLIBNAME	"debug"
LUALIB_API int (luaopen_debug) (lua_State *L);

#define LUA_LOADLIBNAME	"package"
LUALIB_API int (luaopen_package) (lua_State *L);

#endif

/* open all previous libraries */
void luaL_openlibs(lua_State * L);

#endif /* __LUALIB_H__ */
