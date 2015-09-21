#ifndef __FS_H__
#define __FS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fs/vfs/vfs.h>

/*
 * filesystem structure
 */
struct filesystem_t
{
	/* filesystem name */
	const char * name;

	/* pointer to vfs operation */
	struct vfsops_t * vfsops;
};

/*
 * the list of fstab
 */
struct fs_list
{
	struct filesystem_t * fs;
	struct list_head entry;
};

bool_t filesystem_register(struct filesystem_t * fs);
bool_t filesystem_unregister(struct filesystem_t * fs);
struct filesystem_t * filesystem_search(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __FS_H__ */
