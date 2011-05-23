#ifndef __LUNDUMP_H__
#define __LUNDUMP_H__

#include <lua/luacfg.h>
#include <lua/lobject.h>
#include <lua/lzio.h>

/* for header of binary files -- this is Lua 5.1 */
#define LUAC_VERSION			0x51

/* for header of binary files -- this is the official format */
#define LUAC_FORMAT				0

/* size of header of binary files */
#define LUAC_HEADERSIZE			12


/* load one chunk; from lundump.c */
Proto * luaU_undump(lua_State * L, ZIO * Z, Mbuffer * buff, const char * name);

/* make header; from lundump.c */
void luaU_header(char * h);

/* dump one chunk; from ldump.c */
int luaU_dump(lua_State * L, const Proto * f, lua_Writer w, void * data, int strip);

#endif /* __LUNDUMP_H__ */
