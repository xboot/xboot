#ifndef __FRAMEWORK_VM_H__
#define __FRAMEWORK_VM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <framework/framework.h>

struct vm_t {
	lua_State * lua;
};

struct vm_t * vm_alloc(const char * path, int argc, char * argv[]);
void vm_free(struct vm_t * vm);
int vm_run(struct vm_t * vm);
int vm_exec(const char * path, int argc, char * argv[]);

#ifdef __cplusplus
}
#endif

#endif /* __FRAMEWORK_VM_H__ */
