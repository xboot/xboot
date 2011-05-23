#ifndef __LTABLE_H__
#define __LTABLE_H__

#include <lua/luacfg.h>
#include <lua/lobject.h>

#define gnode(t,i)			(&(t)->node[i])
#define gkey(n)				(&(n)->i_key.nk)
#define gval(n)				(&(n)->i_val)
#define gnext(n)			((n)->i_key.nk.next)
#define key2tval(n)			(&(n)->i_key.tvk)


const TValue *luaH_getnum(Table *t, int key);
TValue *luaH_setnum(lua_State *L, Table *t, int key);
const TValue *luaH_getstr(Table *t, TString *key);
TValue *luaH_setstr(lua_State *L, Table *t, TString *key);
const TValue *luaH_get(Table *t, const TValue *key);
TValue *luaH_set(lua_State *L, Table *t, const TValue *key);
Table *luaH_new(lua_State *L, int narray, int lnhash);
void luaH_resizearray(lua_State *L, Table *t, int nasize);
void luaH_free(lua_State *L, Table *t);
int luaH_next(lua_State *L, Table *t, StkId key);
int luaH_getn(Table *t);

#if defined(LUA_DEBUG)
Node *luaH_mainposition(const Table *t, const TValue *key);
int luaH_isdummy(Node *n);
#endif

#endif /* __LTABLE_H__ */
