#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandboxlinux.h>

static SDL_Surface * screen = NULL;

int sandbox_linux_sdl_fb_open(void)
{
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0)
		return -1;

	screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL);
	if(!screen)
	{
		SDL_Quit();
		return -1;
	}

	SDL_FillRect(screen, NULL, 0xff0000);
	SDL_Flip(screen);
	return 0;
}

int sandbox_linux_sdl_fb_close(void)
{
	SDL_Quit();
	return 0;
}

int sandbox_linux_sdl_surface_create(struct linux_fb_surface_t * surface)
{
	SDL_Surface * face = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL, 640, 480, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0);
	if(!face)
		return -1;

	surface->width = face->w;
	surface->height = face->h;
	surface->pitch = face->pitch;
	surface->pixels = face->pixels;
	surface->surface = face;

	return 0;
}

int sandbox_linux_sdl_surface_destroy(struct linux_fb_surface_t * surface)
{
	if(surface)
		SDL_FreeSurface(surface->surface);
	return 0;
}

int sandbox_linux_sdl_present(struct linux_fb_surface_t * surface)
{
	SDL_BlitSurface(surface->surface, NULL, screen, NULL);
	SDL_Flip(screen);
	return 0;
}
