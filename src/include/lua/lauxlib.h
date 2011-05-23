#ifndef __LAUXLIB_H__
#define __LAUXLIB_H__

#include <lua/luacfg.h>
#include <lua/llimits.h>
#include <lua/lua.h>


#if defined(LUA_COMPAT_GETN)
int (luaL_getn)(lua_State *L, int t);
void (luaL_setn)(lua_State *L, int t, int n);
#else
#define luaL_getn(L,i)			((int)lua_objlen(L, i))
#define luaL_setn(L,i,j)		((void)0)
#endif

#if defined(LUA_COMPAT_OPENLIB)
#define luaI_openlib			luaL_openlib
#endif

/* extra error code for `luaL_load' */
#define LUA_ERRFILE				(LUA_ERRERR + 1)

typedef struct luaL_Reg {
	const char *name;
	lua_CFunction func;
} luaL_Reg;


void (luaI_openlib)(lua_State *L, const char *libname, const luaL_Reg *l, int nup);
void ( luaL_register)(lua_State *L, const char *libname, const luaL_Reg *l);
int ( luaL_getmetafield)(lua_State *L, int obj, const char *e);
int ( luaL_callmeta)(lua_State *L, int obj, const char *e);
int ( luaL_typerror)(lua_State *L, int narg, const char *tname);
int ( luaL_argerror)(lua_State *L, int numarg, const char *extramsg);
const char *( luaL_checklstring)(lua_State *L, int numArg, size_t *l);
const char *( luaL_optlstring)(lua_State *L, int numArg, const char *def, size_t *l);
lua_Number ( luaL_checknumber)(lua_State *L, int numArg);
lua_Number ( luaL_optnumber)(lua_State *L, int nArg, lua_Number def);

lua_Integer ( luaL_checkinteger)(lua_State *L, int numArg);
lua_Integer ( luaL_optinteger)(lua_State *L, int nArg, lua_Integer def);

void ( luaL_checkstack)(lua_State *L, int sz, const char *msg);
void ( luaL_checktype)(lua_State *L, int narg, int t);
void ( luaL_checkany)(lua_State *L, int narg);

int ( luaL_newmetatable)(lua_State *L, const char *tname);
void *( luaL_checkudata)(lua_State *L, int ud, const char *tname);

void ( luaL_where)(lua_State *L, int lvl);
int ( luaL_error)(lua_State *L, const char *fmt, ...);

int ( luaL_checkoption)(lua_State *L, int narg, const char *def, const char * const lst[]);

int ( luaL_ref)(lua_State *L, int t);
void ( luaL_unref)(lua_State *L, int t, int ref);

int ( luaL_loadfile)(lua_State *L, const char *filename);
int ( luaL_loadbuffer)(lua_State *L, const char *buff, size_t sz, const char *name);
int ( luaL_loadstring)(lua_State *L, const char *s);

lua_State *( luaL_newstate)(void);

const char *( luaL_gsub)(lua_State *L, const char *s, const char *p, const char *r);

const char *( luaL_findtable)(lua_State *L, int idx, const char *fname,	int szhint);

/*
 * some useful macros
 */
#define luaL_argcheck(L, cond,numarg,extramsg)	((void)((cond) || luaL_argerror(L, (numarg), (extramsg))))
#define luaL_checkstring(L,n)					(luaL_checklstring(L, (n), NULL))
#define luaL_optstring(L,n,d)					(luaL_optlstring(L, (n), (d), NULL))
#define luaL_checkint(L,n)						((int)luaL_checkinteger(L, (n)))
#define luaL_optint(L,n,d)						((int)luaL_optinteger(L, (n), (d)))
#define luaL_checklong(L,n)						((long)luaL_checkinteger(L, (n)))
#define luaL_optlong(L,n,d)						((long)luaL_optinteger(L, (n), (d)))

#define luaL_typename(L,i)						lua_typename(L, lua_type(L,(i)))

#define luaL_dofile(L, fn) 						(luaL_loadfile(L, fn) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define luaL_dostring(L, s) 					(luaL_loadstring(L, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define luaL_getmetatable(L,n)					(lua_getfield(L, LUA_REGISTRYINDEX, (n)))

#define luaL_opt(L,f,n,d)						(lua_isnoneornil(L,(n)) ? (d) : f(L,(n)))

/*
 * Generic Buffer manipulation
 */
typedef struct luaL_Buffer {
	char *p;				/* current position in buffer */
	int lvl; 				/* number of strings in the stack (level) */
	lua_State *L;
	char buffer[LUAL_BUFFERSIZE];
} luaL_Buffer;

#define luaL_addchar(B,c) 		((void)((B)->p < ((B)->buffer+LUAL_BUFFERSIZE) || luaL_prepbuffer(B)), (*(B)->p++ = (char)(c)))

/* compatibility only */
#define luaL_putchar(B,c)		luaL_addchar(B,c)

#define luaL_addsize(B,n)		((B)->p += (n))


void ( luaL_buffinit)(lua_State *L, luaL_Buffer *B);
char *( luaL_prepbuffer)(luaL_Buffer *B);
void ( luaL_addlstring)(luaL_Buffer *B, const char *s, size_t l);
void ( luaL_addstring)(luaL_Buffer *B, const char *s);
void ( luaL_addvalue)(luaL_Buffer *B);
void ( luaL_pushresult)(luaL_Buffer *B);

/* compatibility with ref system */

/* pre-defined references */
#define LUA_NOREF       		(-2)
#define LUA_REFNIL      		(-1)

#define lua_ref(L,lock)			((lock) ? luaL_ref(L, LUA_REGISTRYINDEX) : \
	(lua_pushstring(L, "unlocked references are obsolete"), lua_error(L), 0))

#define lua_unref(L,ref)		luaL_unref(L, LUA_REGISTRYINDEX, (ref))

#define lua_getref(L,ref)		lua_rawgeti(L, LUA_REGISTRYINDEX, (ref))

#define luaL_reg				luaL_Reg

#endif /* __LAUXLIB_H__ */


