#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandboxlinux.h>

struct {
	SDL_Window * window;
	SDL_Surface * screen;
} __fb;

void sandbox_linux_sdl_fb_init(int width, int height)
{
	__fb.window = SDL_CreateWindow("Xboot Runtime Environment", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if(!__fb.window)
		return;

	__fb.screen = SDL_GetWindowSurface(__fb.window);
	if(!__fb.screen)
		return;
}

void sandbox_linux_sdl_fb_exit(void)
{
	if(__fb.window)
		SDL_DestroyWindow(__fb.window);

	if(__fb.screen)
		SDL_FreeSurface(__fb.screen);
}

int sandbox_linux_sdl_fb_surface_create(struct sandbox_fb_surface_t * surface, int width, int height)
{
	SDL_Surface * face;
	Uint32 r, g, b, a;
	int bpp;

	SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888, &bpp, &r, &g, &b, &a);
	face = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, bpp, r, g, b, a);
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
	SDL_BlitSurface(surface->surface, NULL, __fb.screen, NULL);
	SDL_UpdateWindowSurface(__fb.window);
	return 0;
}

void sandbox_linux_sdl_fb_set_backlight(int brightness)
{
	SDL_SetWindowBrightness(__fb.window, brightness / 1024.0);
}

int sandbox_linux_sdl_fb_get_backlight(void)
{
	return (int)(SDL_GetWindowBrightness(__fb.window) * 1024);
}
