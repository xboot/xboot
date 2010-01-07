#ifndef __PROC_H__
#define __PROC_H__

#include <configs.h>
#include <default.h>
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
	x_s32 (*read)(x_u8 * buf, x_s32 offset, x_s32 count);
};

/*
 * the list of proc
 */
struct proc_list
{
	struct proc * proc;
	struct list_head entry;
};


x_bool proc_register(struct proc * proc);
x_bool proc_unregister(struct proc * proc);
struct proc * proc_search(const char *name);

#endif /* __PROC_H__ */
