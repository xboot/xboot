#ifndef __PROC_H__
#define __PROC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

/*
 * the struct of proc.
 */
struct proc
{
	/* the proc name */
	const char * name;

	/* proc read */
	s32_t (*read)(u8_t * buf, s32_t offset, s32_t count);
};

/*
 * the list of proc
 */
struct proc_list
{
	struct proc * proc;
	struct list_head entry;
};

bool_t proc_register(struct proc * proc);
bool_t proc_unregister(struct proc * proc);
struct proc * proc_search(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* __PROC_H__ */
