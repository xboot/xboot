#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandboxlinux.h>

static Uint32 tick_timer_callback(Uint32 interval, void * param)
{
	if(param)
		((void (*)(void))param)();
	return interval;
}

void sandbox_linux_sdl_timer_init(int delay, void (*cb)(void))
{
	SDL_AddTimer(delay, tick_timer_callback, cb);
}
