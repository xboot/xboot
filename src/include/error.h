#ifndef __ERROR_H__
#define __ERROR_H__

#include <xboot.h>


#define ENOERR			 0		/* no error */
#define EBADF			-1		/* bad file handle */
#define EACCES			-2		/* permission denied */
#define EEXIST			-3		/* file exists */
#define EISDIR			-4		/* is a directory */
#define ENOTDIR			-5		/* not a directory */
#define ENOENT			-6		/* no such entity */

#define EINVAL			-11		/* invalid argument */
#define ENODEV			-12		/* no such device */
#define EBUSY			-13		/* resource busy */
#define EIO				-14		/* i/o error */
#define ENOMEM			-15		/* out of memory */
#define EINTR			-16		/* operation interrupted */
#define EAGAIN			-17		/* again */

#endif /* __ERROR_H__ */
