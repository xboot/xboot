/*
 * system call interface
 */

#include <syscall.h>

extern int syscall(int * reg);

/*
 * syscall_sync
 */
void syscall_sync(void)
{
	int reg[10];

	reg[0] = SYSCALL_FILESYSTEM_SYNC;

	syscall(reg);
	return;
}
