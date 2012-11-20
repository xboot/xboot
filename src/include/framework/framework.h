#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

struct vm_t {
	/* lua virtual machine */
	lua_State * lua;

	/* xfs context */
	struct xfs_context_t * xfs;
};

struct vm_t * vm_alloc(const char * path, int argc, char * argv[]);
void vm_free(struct vm_t * vm);
int vm_run(struct vm_t * vm);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_H__ */
