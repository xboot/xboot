#ifndef __XFS_PLATFORM_H__
#define __XFS_PLATFORM_H__

#include <xboot.h>

bool_t __xfs_platform_init(void);
bool_t __xfs_platform_exit(void);

void * __xfs_platform_create_mutex(void);
void __xfs_platform_destory_mutex(void * mutex);
void __xfs_platform_lock_mutex(void * mutex);
void __xfs_platform_unlock_mutex(void * mutex);

#endif /* __XFS_PLATFORM_H__ */
