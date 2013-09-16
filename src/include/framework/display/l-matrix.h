#ifndef __FRAMEWORK_L_MATRIX_H__
#define __FRAMEWORK_L_MATRIX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define	MT_NAME_MATRIX		"mt_name_matrix"

int luaopen_matrix(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_L_MATRIX_H__ */
