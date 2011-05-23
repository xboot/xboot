#ifndef __LUA_H__
#define __LUA_H__

#include <lua/luacfg.h>

#define LUA_VERSION				"Lua 5.1"
#define LUA_RELEASE				"Lua 5.1.4"
#define LUA_VERSION_NUM			501
#define LUA_COPYRIGHT			"Copyright (C) 1994-2008 Lua.org, PUC-Rio"
#define LUA_AUTHORS				"R. Ierusalimschy, L. H. de Figueiredo & W. Celes"

/*
 * mark for precompiled code (`<esc>Lua')
 */
#define	LUA_SIGNATURE			"\033Lua"

/*
 * option for multiple returns in `lua_pcall' and `lua_call'
 */
#define LUA_MULTRET				(-1)

/*
 * pseudo-indices
 */
#define LUA_REGISTRYINDEX		(-10000)
#define LUA_ENVIRONINDEX		(-10001)
#define LUA_GLOBALSINDEX		(-10002)
#define lua_upvalueindex(i)		(LUA_GLOBALSINDEX - (i))

/*
 * thread status; 0 is OK
 */
#define LUA_YIELD				1
#define LUA_ERRRUN				2
#define LUA_ERRSYNTAX			3
#define LUA_ERRMEM				4
#define LUA_ERRERR				5

/*
 * basic types
 */
#define LUA_TNONE				(-1)

#define LUA_TNIL				0
#define LUA_TBOOLEAN			1
#define LUA_TLIGHTUSERDATA		2
#define LUA_TNUMBER				3
#define LUA_TSTRING				4
#define LUA_TTABLE				5
#define LUA_TFUNCTION			6
#define LUA_TUSERDATA			7
#define LUA_TTHREAD				8

/*
 * garbage-collection options
 */
#define LUA_GCSTOP				0
#define LUA_GCRESTART			1
#define LUA_GCCOLLECT			2
#define LUA_GCCOUNT				3
#define LUA_GCCOUNTB			4
#define LUA_GCSTEP				5
#define LUA_GCSETPAUSE			6
#define LUA_GCSETSTEPMUL		7

/*
 * Event codes
 */
#define LUA_HOOKCALL			0
#define LUA_HOOKRET				1
#define LUA_HOOKLINE			2
#define LUA_HOOKCOUNT			3
#define LUA_HOOKTAILRET			4

/*
 * Event masks
 */
#define LUA_MASKCALL			(1 << LUA_HOOKCALL)
#define LUA_MASKRET				(1 << LUA_HOOKRET)
#define LUA_MASKLINE			(1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT			(1 << LUA_HOOKCOUNT)

/*
 * minimum Lua stack available to a C function
 */
#define LUA_MINSTACK			20

/*
 * type of numbers in Lua
 */
typedef LUA_NUMBER				lua_Number;

/*
 * type for integer functions
 */
typedef LUA_INTEGER				lua_Integer;

typedef struct lua_State	lua_State;
typedef int (*lua_CFunction) (lua_State *L);

/*
 * functions that read/write blocks when loading/dumping Lua chunks
 */
typedef const char * (*lua_Reader) (lua_State *L, void *ud, size_t *sz);

typedef int (*lua_Writer) (lua_State *L, const void* p, size_t sz, void* ud);

/*
 * prototype for memory-allocation functions
 */
typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);



/*
** state manipulation
*/
 lua_State *(lua_newstate) (lua_Alloc f, void *ud);
 void       (lua_close) (lua_State *L);
 lua_State *(lua_newthread) (lua_State *L);

 lua_CFunction (lua_atpanic) (lua_State *L, lua_CFunction panicf);


/*
** basic stack manipulation
*/
 int   (lua_gettop) (lua_State *L);
 void  (lua_settop) (lua_State *L, int idx);
 void  (lua_pushvalue) (lua_State *L, int idx);
 void  (lua_remove) (lua_State *L, int idx);
 void  (lua_insert) (lua_State *L, int idx);
 void  (lua_replace) (lua_State *L, int idx);
 int   (lua_checkstack) (lua_State *L, int sz);

 void  (lua_xmove) (lua_State *from, lua_State *to, int n);


/*
** access functions (stack -> C)
*/

 int             (lua_isnumber) (lua_State *L, int idx);
 int             (lua_isstring) (lua_State *L, int idx);
 int             (lua_iscfunction) (lua_State *L, int idx);
 int             (lua_isuserdata) (lua_State *L, int idx);
 int             (lua_type) (lua_State *L, int idx);
 const char     *(lua_typename) (lua_State *L, int tp);

 int            (lua_equal) (lua_State *L, int idx1, int idx2);
 int            (lua_rawequal) (lua_State *L, int idx1, int idx2);
 int            (lua_lessthan) (lua_State *L, int idx1, int idx2);

 lua_Number      (lua_tonumber) (lua_State *L, int idx);
 lua_Integer     (lua_tointeger) (lua_State *L, int idx);
 int             (lua_toboolean) (lua_State *L, int idx);
 const char     *(lua_tolstring) (lua_State *L, int idx, size_t *len);
 size_t          (lua_objlen) (lua_State *L, int idx);
 lua_CFunction   (lua_tocfunction) (lua_State *L, int idx);
 void	       *(lua_touserdata) (lua_State *L, int idx);
 lua_State      *(lua_tothread) (lua_State *L, int idx);
 const void     *(lua_topointer) (lua_State *L, int idx);


/*
** push functions (C -> stack)
*/
 void  (lua_pushnil) (lua_State *L);
 void  (lua_pushnumber) (lua_State *L, lua_Number n);
 void  (lua_pushinteger) (lua_State *L, lua_Integer n);
 void  (lua_pushlstring) (lua_State *L, const char *s, size_t l);
 void  (lua_pushstring) (lua_State *L, const char *s);
 const char *(lua_pushvfstring) (lua_State *L, const char *fmt,
                                                      va_list argp);
 const char *(lua_pushfstring) (lua_State *L, const char *fmt, ...);
 void  (lua_pushcclosure) (lua_State *L, lua_CFunction fn, int n);
 void  (lua_pushboolean) (lua_State *L, int b);
 void  (lua_pushlightuserdata) (lua_State *L, void *p);
 int   (lua_pushthread) (lua_State *L);


/*
** get functions (Lua -> stack)
*/
 void  (lua_gettable) (lua_State *L, int idx);
 void  (lua_getfield) (lua_State *L, int idx, const char *k);
 void  (lua_rawget) (lua_State *L, int idx);
 void  (lua_rawgeti) (lua_State *L, int idx, int n);
 void  (lua_createtable) (lua_State *L, int narr, int nrec);
 void *(lua_newuserdata) (lua_State *L, size_t sz);
 int   (lua_getmetatable) (lua_State *L, int objindex);
 void  (lua_getfenv) (lua_State *L, int idx);


/*
** set functions (stack -> Lua)
*/
 void  (lua_settable) (lua_State *L, int idx);
 void  (lua_setfield) (lua_State *L, int idx, const char *k);
 void  (lua_rawset) (lua_State *L, int idx);
 void  (lua_rawseti) (lua_State *L, int idx, int n);
 int   (lua_setmetatable) (lua_State *L, int objindex);
 int   (lua_setfenv) (lua_State *L, int idx);


/*
** `load' and `call' functions (load and run Lua code)
*/
 void  (lua_call) (lua_State *L, int nargs, int nresults);
 int   (lua_pcall) (lua_State *L, int nargs, int nresults, int errfunc);
 int   (lua_cpcall) (lua_State *L, lua_CFunction func, void *ud);
 int   (lua_load) (lua_State *L, lua_Reader reader, void *dt,
                                        const char *chunkname);

 int (lua_dump) (lua_State *L, lua_Writer writer, void *data);


/*
** coroutine functions
*/
 int  (lua_yield) (lua_State *L, int nresults);
 int  (lua_resume) (lua_State *L, int narg);
 int  (lua_status) (lua_State *L);

/*
** garbage-collection function and options
*/



 int (lua_gc) (lua_State *L, int what, int data);


/*
** miscellaneous functions
*/

 int   (lua_error) (lua_State *L);

 int   (lua_next) (lua_State *L, int idx);

 void  (lua_concat) (lua_State *L, int n);

 lua_Alloc (lua_getallocf) (lua_State *L, void **ud);
 void lua_setallocf (lua_State *L, lua_Alloc f, void *ud);



/* 
** ===============================================================
** some useful macros
** ===============================================================
*/

#define lua_pop(L,n)		lua_settop(L, -(n)-1)

#define lua_newtable(L)		lua_createtable(L, 0, 0)

#define lua_register(L,n,f) (lua_pushcfunction(L, (f)), lua_setglobal(L, (n)))

#define lua_pushcfunction(L,f)	lua_pushcclosure(L, (f), 0)

#define lua_strlen(L,i)		lua_objlen(L, (i))

#define lua_isfunction(L,n)	(lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L,n)	(lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L,n)	(lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)		(lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L,n)	(lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isthread(L,n)	(lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L,n)		(lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n)	(lua_type(L, (n)) <= 0)

#define lua_pushliteral(L, s)	\
	lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)

#define lua_setglobal(L,s)	lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_getglobal(L,s)	lua_getfield(L, LUA_GLOBALSINDEX, (s))

#define lua_tostring(L,i)	lua_tolstring(L, (i), NULL)



/*
** compatibility macros and functions
*/

#define lua_open()	luaL_newstate()

#define lua_getregistry(L)	lua_pushvalue(L, LUA_REGISTRYINDEX)

#define lua_getgccount(L)	lua_gc(L, LUA_GCCOUNT, 0)

#define lua_Chunkreader		lua_Reader
#define lua_Chunkwriter		lua_Writer


/* hack */
 void lua_setlevel	(lua_State *from, lua_State *to);


/*
** {======================================================================
** Debug API
** =======================================================================
*/




typedef struct lua_Debug lua_Debug;  /* activation record */


/* Functions to be called by the debuger in specific events */
typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar);


 int lua_getstack (lua_State *L, int level, lua_Debug *ar);
 int lua_getinfo (lua_State *L, const char *what, lua_Debug *ar);
 const char *lua_getlocal (lua_State *L, const lua_Debug *ar, int n);
 const char *lua_setlocal (lua_State *L, const lua_Debug *ar, int n);
 const char *lua_getupvalue (lua_State *L, int funcindex, int n);
 const char *lua_setupvalue (lua_State *L, int funcindex, int n);

 int lua_sethook (lua_State *L, lua_Hook func, int mask, int count);
 lua_Hook lua_gethook (lua_State *L);
 int lua_gethookmask (lua_State *L);
 int lua_gethookcount (lua_State *L);


struct lua_Debug {
  int event;
  const char *name;	/* (n) */
  const char *namewhat;	/* (n) `global', `local', `field', `method' */
  const char *what;	/* (S) `Lua', `C', `main', `tail' */
  const char *source;	/* (S) */
  int currentline;	/* (l) */
  int nups;		/* (u) number of upvalues */
  int linedefined;	/* (S) */
  int lastlinedefined;	/* (S) */
  char short_src[LUA_IDSIZE]; /* (S) */
  /* private part */
  int i_ci;  /* active function */
};

#endif /* __LUA_H__ */
