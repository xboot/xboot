#ifndef __LSTRING_H__
#define __LSTRING_H__

#include <lua/luacfg.h>
#include <lua/lgc.h>
#include <lua/lobject.h>
#include <lua/lstate.h>

#define sizestring(s)				(sizeof(union TString)+((s)->len+1)*sizeof(char))
#define sizeudata(u)				(sizeof(union Udata)+(u)->len)
#define luaS_new(L, s)				(luaS_newlstr(L, s, strlen(s)))
#define luaS_newliteral(L, s)		(luaS_newlstr(L, "" s, (sizeof(s)/sizeof(char))-1))
#define luaS_fix(s)					l_setbit((s)->tsv.marked, FIXEDBIT)


void luaS_resize(lua_State * L, int newsize);
TString * luaS_newlstr(lua_State * L, const char * str, size_t l);
Udata * luaS_newudata (lua_State * L, size_t s, Table * e);

#endif /* __LSTRING_H__ */
