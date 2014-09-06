#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandboxlinux.h>

static SDL_Surface * __screen = NULL;
static int __brightness = 0;

void sandbox_linux_sdl_fb_init(int width, int height, int bpp)
{
	__screen = SDL_SetVideoMode(width, height, bpp, SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL | SDL_DOUBLEBUF);
	if(!__screen)
		return;

	SDL_WM_SetCaption("xboot sandbox for linux", "xboot");
	SDL_FillRect(__screen, NULL, 0x000000);
	SDL_Flip(__screen);
}

int sandbox_linux_sdl_fb_surface_create(struct sandbox_fb_surface_t * surface, int width, int height, int bpp)
{
	SDL_Surface * face = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL | SDL_DOUBLEBUF, width, height, bpp, 0x00ff0000, 0x0000ff00, 0x000000ff, 0);
	if(!face)
		return -1;

	surface->width = face->w;
	surface->height = face->h;
	surface->pitch = face->pitch;
	surface->pixels = face->pixels;
	surface->surface = face;

	return 0;
}

int sandbox_linux_sdl_fb_surface_destroy(struct sandbox_fb_surface_t * surface)
{
	if(surface)
		SDL_FreeSurface(surface->surface);
	return 0;
}

int sandbox_linux_sdl_fb_surface_present(struct sandbox_fb_surface_t * surface)
{
	SDL_BlitSurface(surface->surface, NULL, __screen, NULL);
	SDL_Flip(__screen);
	return 0;
}

void sandbox_linux_sdl_fb_set_backlight(int brightness)
{
	__brightness = brightness;
}

int sandbox_linux_sdl_fb_get_backlight(void)
{
	return __brightness;
}
