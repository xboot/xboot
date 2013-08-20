#ifndef __PROC_H__
#define __PROC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct proc_t
{
	/* The proc name */
	const char * name;

	/* Proc read */
	s32_t (*read)(u8_t * buf, s32_t offset, s32_t count);
};

struct proc_list_t
{
	struct proc_t * proc;
	struct list_head entry;
};

bool_t proc_register(struct proc_t * proc);
bool_t proc_unregister(struct proc_t * proc);
struct proc_t * proc_search(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __PROC_H__ */
