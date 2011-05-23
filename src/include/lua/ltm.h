#ifndef __LTM_H__
#define __LTM_H__

#include <lua/luacfg.h>
#include <lua/lobject.h>

/*
 * WARNING: if you change the order of this enumeration,
 * grep "ORDER TM"
 */
typedef enum {
	TM_INDEX,
	TM_NEWINDEX,
	TM_GC,
	TM_MODE,
	TM_EQ,		/* last tag method with `fast' access */
	TM_ADD,
	TM_SUB,
	TM_MUL,
	TM_DIV,
	TM_MOD,
	TM_POW,
	TM_UNM,
	TM_LEN,
	TM_LT,
	TM_LE,
	TM_CONCAT,
	TM_CALL,
	TM_N		/* number of elements in the enum */
} TMS;

#define gfasttm(g,et,e) ((et) == NULL ? NULL : \
	((et)->flags & (1u<<(e))) ? NULL : luaT_gettm(et, e, (g)->tmname[e]))

#define fasttm(l,et,e)	\
	gfasttm(G(l), et, e)

extern const char *const luaT_typenames[];

const TValue * luaT_gettm(Table * events, TMS event, TString * ename);
const TValue * luaT_gettmbyobj(lua_State * L, const TValue * o, TMS event);
void luaT_init(lua_State * L);

#endif /* __LTM_H__ */
