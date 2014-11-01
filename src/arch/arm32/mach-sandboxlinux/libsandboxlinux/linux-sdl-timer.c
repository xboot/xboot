#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandboxlinux.h>

static SDL_TimerID __timer_id = NULL;

static Uint32 tick_timer_callback(Uint32 interval, void * param)
{
	if(param)
		((void (*)(void))param)();
	return interval;
}

void sandbox_linux_sdl_timer_init(int delay, void (*cb)(void))
{
	__timer_id = SDL_AddTimer(delay, tick_timer_callback, cb);
}

void sandbox_linux_sdl_timer_exit(void)
{
	if(__timer_id)
		SDL_RemoveTimer(__timer_id);
}
