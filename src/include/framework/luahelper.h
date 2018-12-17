#ifndef __FRAMEWORK_LUAHELPER_H__
#define __FRAMEWORK_LUAHELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>
#include <lapi.h>
#include <lauxlib.h>
#include <lualib.h>
#include <framework/vm.h>

static inline struct vmctx_t * luahelper_vmctx(lua_State * L)
{
	return (struct vmctx_t *)(G(L)->ud);
}

void luahelper_dump_stack(lua_State * L);
int luahelper_deepcopy_table(lua_State * L);
const char * luahelper_get_strfield(lua_State * L, const char * key, const char * def);
lua_Number luahelper_get_numfield(lua_State * L, const char * key, lua_Number def);
lua_Integer luahelper_get_intfield(lua_State * L, const char * key, lua_Integer def);
void luahelper_set_strfield(lua_State * L, const char * key, const char * value);
void luahelper_set_numfield(lua_State * L, const char * key, lua_Number value);
void luahelper_set_intfield(lua_State * L, const char * key, lua_Integer value);
void luahelper_package_searcher(lua_State * L, lua_CFunction f, int pos);
void luahelper_package_path(lua_State * L, const char * path);
void luahelper_package_cpath(lua_State * L, const char * cpath);
void luahelper_preload(lua_State * L, const char * name, lua_CFunction f);
void luahelper_create_metatable(lua_State * L, const char * name, const luaL_Reg * funcs);
void luahelper_create_class(lua_State * L, const char * parent, const luaL_Reg * funcs);
int luahelper_pcall(lua_State * L, int narg, int nres);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_LUAHELPER_H__ */
