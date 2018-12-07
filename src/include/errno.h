#ifndef __ERRNO_H__
#define __ERRNO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot/module.h>

enum {
	ENOERR			=  0,
	EDOM			= -1,
	ERANGE			= -2,
	ENOSYS			= -3,
	EINVAL			= -4,
	ESPIPE			= -5,
	EBADF			= -6,
	ENOMEM			= -7,
	EACCES			= -8,
	ENFILE			= -9,
	EMFILE			= -10,
	ENAMETOOLONG	= -11,
	ELOOP			= -12,
	ENOMSG			= -13,
	E2BIG			= -14,
	EINTR			= -15,
	EILSEQ			= -16,
	ENOEXEC			= -17,
	ENOENT			= -18,
	EPROTOTYPE		= -19,
	ESRCH			= -20,
	EPERM			= -21,
	ENOTDIR			= -22,
	ESTALE			= -23,
	EISDIR			= -24,
	EOPNOTSUPP		= -25,
	ENOTTY			= -26,
	EAGAIN			= -27,
	EIO				= -28,
	ENOSPC			= -29,
	EEXIST			= -30,
	EBUSY			= -31,
	EOVERFLOW		= -32,
};

/*
 * A macro expanding to the errno l-value
 */
#define errno 	(*__task_errno_location())

/*
 * Internal function returning the address of the task-specific errno
 */
volatile int * __task_errno_location(void);

/*
 * Returns a string containing a message derived from an error code
 */
char * strerror(int e);

#ifdef __cplusplus
}
#endif

#endif /* __ERRNO_H__ */
