#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sandboxlinux.h>

ssize_t sandbox_linux_read(int fd, void * buf, size_t count)
{
	ssize_t ret = read(fd, buf, count);
	return (ret > 0) ? ret: 0;
}

ssize_t sandbox_linux_read_nonblock(int fd, void * buf, size_t count)
{
	ssize_t ret;
	int flags;

	flags = fcntl(fd, F_GETFL);
	if(flags == -1)
		return 0;

	if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		return 0;

	ret = sandbox_linux_read(fd, buf, count);

	if(fcntl(fd, F_SETFL, flags) == -1)
		return 0;

	return ret;
}

ssize_t sandbox_linux_write(int fd, const void * buf, size_t count)
{
	ssize_t ret = write(fd, buf, count);
	return (ret > 0) ? ret: 0;
}

off_t sandbox_linux_lseek(int fd, off_t offset)
{
	return lseek(fd, offset, SEEK_SET);
}

int sandbox_linux_execve(const char * filename, char * const argv[], char * const envp[])
{
	pid_t pid, tpid;
	int execve_status;

	pid = fork();

	if (pid == -1)
	{
		perror("linux_execve");
		return pid;
	}
	else if (pid == 0)
	{
		exit(execve(filename, argv, envp));
	}
	else
	{
		do {
			tpid = wait(&execve_status);
		} while(tpid != pid);

		return execve_status;
	}
}
