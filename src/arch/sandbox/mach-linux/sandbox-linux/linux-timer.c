#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sandbox-linux.h>

static pthread_t timer;

static void  * timer_thread(void * ptr)
{
	while(1)
	{
		usleep(1000 * 10);
		((void (*)(void))ptr)();
	}
	return 0;
}

void sandbox_linux_timer_start(void (*cb)(void))
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&timer, &attr, timer_thread, cb);
}

void sandbox_linux_timer_stop(void)
{
	pthread_cancel(timer);
}
