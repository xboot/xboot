#include <x.h>
#include <sandbox.h>

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
