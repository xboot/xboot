#include <x.h>
#include <sandbox.h>

struct sandbox_timer_context_t {
	struct {
		void (*cb)(void *);
		void * data;
	} tcd;
	struct sigevent sev;
	struct itimerspec its;
	timer_t tid;
};
static struct sandbox_timer_context_t tctx;

static void signal_timer_handler(int signum)
{
	tctx.its.it_value.tv_sec = 0;
	tctx.its.it_value.tv_nsec = 0;
	timer_settime(tctx.tid, 0, &tctx.its, NULL);
	if(tctx.tcd.cb)
		tctx.tcd.cb(tctx.tcd.data);
}

void sandbox_timer_init(void)
{
	tctx.sev.sigev_notify = SIGEV_SIGNAL;
	tctx.sev.sigev_signo = SIGUSR1;
	tctx.sev.sigev_value.sival_ptr = &tctx.tid;
	signal(SIGUSR1, signal_timer_handler);
	timer_create(CLOCK_MONOTONIC, &tctx.sev, &tctx.tid);
	tctx.tcd.cb = NULL;
	tctx.tcd.data = NULL;
	tctx.its.it_value.tv_sec = 0;
	tctx.its.it_value.tv_nsec = 0;
	tctx.its.it_interval.tv_sec = 0;
	tctx.its.it_interval.tv_nsec = 0;
	timer_settime(tctx.tid, 0, &tctx.its, NULL);
}

void sandbox_timer_exit(void)
{
}

void sandbox_timer_next(uint64_t time, void (*cb)(void *), void * data)
{
	tctx.tcd.cb = cb;
	tctx.tcd.data = data;
	tctx.its.it_value.tv_sec = time / 1000000000ULL;
	tctx.its.it_value.tv_nsec = time % 1000000000ULL;
	timer_settime(tctx.tid, 0, &tctx.its, NULL);
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
