#ifndef __FCNTL_H__
#define __FCNTL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

/*
 * open only flags
 */
#define O_RDONLY				(1<<0)   					/* open for reading only */
#define O_WRONLY				(1<<1)						/* open for writing only */
#define O_RDWR					(O_RDONLY|O_WRONLY)			/* open for reading and writing */
#define O_ACCMODE				(O_RDWR)					/* mask for above modes */

#define O_CREAT					(1<<8)						/* create if nonexistent */
#define O_EXCL					(1<<9)						/* error if already exists */
#define O_NOCTTY				(1<<10)						/* do not assign a controlling terminal */
#define O_TRUNC					(1<<11)						/* truncate to zero length */
#define O_APPEND				(1<<12)						/* set append mode */
#define O_DSYNC					(1<<13)						/* synchronized I/O data integrity writes */
#define O_NONBLOCK				(1<<14)						/* no delay */
#define O_SYNC					(1<<15)						/* synchronized I/O file integrity writes */

/*
 * seek type
 */
#define VFS_SEEK_SET			(0)
#define VFS_SEEK_CUR			(1)
#define VFS_SEEK_END			(2)

#ifdef __cplusplus
}
#endif

#endif /* __FCNTL_H__ */
