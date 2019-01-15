#ifndef __FRAMEWORK_CORE_L_MATRIX_H__
#define __FRAMEWORK_CORE_L_MATRIX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define MT_MATRIX	"__mt_matrix__"

int luaopen_matrix(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_MATRIX_H__ */
