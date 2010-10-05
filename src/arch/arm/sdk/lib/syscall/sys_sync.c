/*
 * system call
 */

void sys_sync(void)
{
	int reg[10];

	reg[0] = SYSCALL_FILESYSTEM_SYNC;

	syscall(reg);
	return;
}
