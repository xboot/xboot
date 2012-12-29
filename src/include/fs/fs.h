#ifndef __FS_H__
#define __FS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>
#include <fs/vfs/vfs.h>

/*
 * filesystem structure
 */
struct filesystem
{
	/* filesystem name */
	const char * name;

	/* pointer to vfs operation */
	struct vfsops * vfsops;
};

/*
 * the list of fstab
 */
struct fs_list
{
	struct filesystem * fs;
	struct list_head entry;
};

bool_t filesystem_register(struct filesystem * fs);
bool_t filesystem_unregister(struct filesystem * fs);
struct filesystem * filesystem_search(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __FS_H__ */
