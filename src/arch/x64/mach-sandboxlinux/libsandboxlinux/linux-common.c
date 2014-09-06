#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sandboxlinux.h>

uint64_t sandbox_linux_get_time(void)
{
	struct timespec ts;
	uint64_t now;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	now = ts.tv_sec * 1000 * 1000 * 1000 + ts.tv_nsec;
	return now;
}

int sandbox_linux_read(int fd, void * buf, size_t count)
{
	ssize_t ret;

	if (count == 0)
		return 0;

	do {
		ret = read(fd, buf, count);

		if (ret == 0)
		{
			printf("read on fd %d returned 0, device gone? - exiting\n", fd);
		}
		else if (ret == -1)
		{
			if (errno == EAGAIN)
				return -errno;
			else if (errno == EINTR)
				continue;
			else {
				printf("read on fd %d returned -1, errno %d - exiting\n", fd, errno);
			}
		}
	} while (ret <= 0);

	return (int)ret;
}

int sandbox_linux_read_nonblock(int fd, void * buf, size_t count)
{
	int oldflags, ret;

	oldflags = fcntl(fd, F_GETFL);
	if (oldflags == -1)
		goto err_out;

	if (fcntl(fd, F_SETFL, oldflags | O_NONBLOCK) == -1)
		goto err_out;

	ret = sandbox_linux_read(fd, buf, count);

	if (fcntl(fd, F_SETFL, oldflags) == -1)
		goto err_out;

	return ret;

err_out:
	perror("fcntl");
	return -1;
}

ssize_t sandbox_linux_write(int fd, const void * buf, size_t count)
{
	return write(fd, buf, count);
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

