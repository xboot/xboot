#ifndef __FRAMEWORK_CORE_L_XFS_H__
#define __FRAMEWORK_CORE_L_XFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/luahelper.h>

#define MT_XFS_FILE		"__mt_xfs_file__"

struct lxfsfile_t {
	struct xfs_file_t * file;
	int closed;
};

int luaopen_xfs(lua_State * L);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_CORE_L_XFS_H__ */
