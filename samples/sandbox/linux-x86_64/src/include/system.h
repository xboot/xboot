#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

struct system_data_t {
	SDL_Surface * screen;
	SDL_Thread * thread;
	int quit;
};

struct system_data_t * get_system_data(void);
void system_init(void);
void system_exit(void);

#endif /* __SYSTEM_H__ */
