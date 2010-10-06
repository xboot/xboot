#ifndef __ARM_SYSCALL_H__
#define __ARM_SYSCALL_H__

enum {
	SYSCALL_FILESYSTEM_SYNC		= 0,
};

void syscall_sync(void);


#endif /* __ARM_SYSCALL_H__ */
