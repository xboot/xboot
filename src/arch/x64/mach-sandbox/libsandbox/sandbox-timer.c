#include <x.h>
#include <sandbox.h>

struct timer_callback_data_t {
	void (*cb)(void *);
	void * data;
};

static struct timer_callback_data_t __tcd;
static struct sigevent __sev;
static struct itimerspec __its;
static timer_t __tid;

static void signal_timer_handler(int signum)
{
	__its.it_value.tv_sec = 0;
	__its.it_value.tv_nsec = 0;
	timer_settime(__tid, 0, &__its, NULL);
	if(__tcd.cb)
		__tcd.cb(__tcd.data);
}

void sandbox_timer_init(void)
{
	__sev.sigev_notify = SIGEV_SIGNAL;
	__sev.sigev_signo = SIGUSR1;
	__sev.sigev_value.sival_ptr = &__tid;
	signal(SIGUSR1, signal_timer_handler);
	timer_create(CLOCK_MONOTONIC, &__sev, &__tid);
	__tcd.cb = NULL;
	__tcd.data = NULL;
	__its.it_value.tv_sec = 0;
	__its.it_value.tv_nsec = 0;
	__its.it_interval.tv_sec = 0;
	__its.it_interval.tv_nsec = 0;
	timer_settime(__tid, 0, &__its, NULL);
}

void sandbox_timer_exit(void)
{
}

void sandbox_timer_next(uint64_t time, void (*cb)(void *), void * data)
{
	__tcd.cb = cb;
	__tcd.data = data;
	__its.it_value.tv_sec = time / 1000000000ULL;
	__its.it_value.tv_nsec = time % 1000000000ULL;
	timer_settime(__tid, 0, &__its, NULL);
}

uint64_t sandbox_timer_count(void)
{
	struct timespec ts;

    if(clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
    	return 0;
    return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

uint64_t sandbox_timer_frequency(void)
{
	return 1000000000ULL;
}
