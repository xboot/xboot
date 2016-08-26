#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandbox.h>

struct sandbox_fb_t {
	SDL_Window * window;
	SDL_Surface * screen;
	char * title;
	int width;
	int height;
};

void * sandbox_sdl_fb_init(const char * title, int width, int height, int fullscreen)
{
	struct sandbox_fb_t * hdl;
	Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;

	if(fullscreen != 0)
		flags |= SDL_WINDOW_FULLSCREEN;

	hdl = malloc(sizeof(struct sandbox_fb_t));
	if(!hdl)
		return NULL;

	hdl->title = strdup(title ? title : "Xboot Runtime Environment");
	hdl->window = SDL_CreateWindow(hdl->title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	if(!hdl->window)
	{
		if(hdl->title)
			free(hdl->title);
		free(hdl);
		return NULL;
	}

	hdl->screen = SDL_GetWindowSurface(hdl->window);
	if(!hdl->screen)
	{
		if(hdl->window)
			SDL_DestroyWindow(hdl->window);
		if(hdl->title)
			free(hdl->title);
		free(hdl);
		return NULL;
	}

	SDL_GetWindowSize(hdl->window, &hdl->width, &hdl->height);
	return hdl;
}

void sandbox_sdl_fb_exit(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;

	if(!hdl)
		return;
	if(hdl->window)
		SDL_DestroyWindow(hdl->window);
	if(hdl->screen)
		SDL_FreeSurface(hdl->screen);
	if(hdl->title)
		free(hdl->title);
	free(hdl);
}

int sandbox_sdl_fb_get_width(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	return hdl->width;
}

int sandbox_sdl_fb_get_height(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	return hdl->height;
}

int sandbox_sdl_fb_surface_create(void * handle, struct sandbox_fb_surface_t * surface)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	SDL_Surface * face;
	Uint32 r, g, b, a;
	int bpp;

	SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888, &bpp, &r, &g, &b, &a);
	face = SDL_CreateRGBSurface(SDL_SWSURFACE, hdl->width, hdl->height, bpp, r, g, b, a);
	if(!face)
		return -1;

	surface->width = face->w;
	surface->height = face->h;
	surface->pitch = face->pitch;
	surface->pixels = face->pixels;
	surface->surface = face;

	return 0;
}

int sandbox_sdl_fb_surface_destroy(void * handle, struct sandbox_fb_surface_t * surface)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;

	if(surface)
		SDL_FreeSurface(surface->surface);
	return 0;
}

int sandbox_sdl_fb_surface_present(void * handle, struct sandbox_fb_surface_t * surface)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;

	SDL_BlitSurface(surface->surface, NULL, hdl->screen, NULL);
	SDL_UpdateWindowSurface(hdl->window);
	return 0;
}

void sandbox_sdl_fb_set_backlight(void * handle, int brightness)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	SDL_SetWindowBrightness(hdl->window, brightness / 1023.0);
}

int sandbox_sdl_fb_get_backlight(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	return (int)(SDL_GetWindowBrightness(hdl->window) * 1023);
}
