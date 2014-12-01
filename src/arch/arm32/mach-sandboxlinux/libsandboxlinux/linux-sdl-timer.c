#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandboxlinux.h>

static SDL_TimerID __timer = 0;

static Uint32 tick_timer_callback(Uint32 interval, void * param)
{
	if(param)
		((void (*)(void))param)();
	return interval;
}

uint64_t sandbox_linux_get_time_counter(void)
{
	return SDL_GetPerformanceCounter();
}

uint64_t sandbox_linux_get_time_frequency(void)
{
	return SDL_GetPerformanceFrequency();
}

void sandbox_linux_sdl_timer_init(int delay, void (*cb)(void))
{
	__timer = SDL_AddTimer(delay, tick_timer_callback, cb);
}

void sandbox_linux_sdl_timer_exit(void)
{
	if(__timer)
		SDL_RemoveTimer(__timer);
}
