#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandbox.h>

struct timer_callback_data_t {
	void (*cb)(void *);
	void * data;
};

void sandbox_sdl_timer_init(void)
{
}

void sandbox_sdl_timer_exit(void)
{
}

static Uint32 timer_callback(Uint32 interval, void * param)
{
	struct timer_callback_data_t * tcd = (struct timer_callback_data_t *)(param);

	((void (*)(void *))tcd->cb)(tcd->data);
	return 0;
}

void sandbox_sdl_timer_set_next(int delay, void (*cb)(void *), void * data)
{
	static struct timer_callback_data_t tcd;

	tcd.cb = cb;
	tcd.data = data;
	SDL_AddTimer(delay, timer_callback, &tcd);
}

uint64_t sandbox_get_time_counter(void)
{
	return SDL_GetPerformanceCounter();
}

uint64_t sandbox_get_time_frequency(void)
{
	return SDL_GetPerformanceFrequency();
}
