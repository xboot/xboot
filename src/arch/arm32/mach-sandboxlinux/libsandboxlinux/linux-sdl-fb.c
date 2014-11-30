#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandboxlinux.h>

struct {
	SDL_Window * window;
	SDL_Renderer * render;
} __fb;

void sandbox_linux_sdl_fb_init(int width, int height)
{
	__fb.window = SDL_CreateWindow("Xboot Runtime Environment", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	if(!__fb.window)
		return;

	__fb.render = SDL_CreateRenderer(__fb.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(!__fb.render)
		return;

    SDL_SetRenderDrawColor(__fb.render, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(__fb.render);
    SDL_RenderPresent(__fb.render);
}

void sandbox_linux_sdl_fb_exit(void)
{
	if(__fb.window)
		SDL_DestroyWindow(__fb.window);

	if(__fb.render)
		SDL_DestroyRenderer(__fb.render);
}

int sandbox_linux_sdl_fb_surface_create(struct sandbox_fb_surface_t * surface, int width, int height)
{
	SDL_Texture * texture = SDL_CreateTexture(__fb.render, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	void * pixels;
	int pitch;

	if(!texture)
		return -1;

	SDL_LockTexture(texture, NULL, &pixels, &pitch);
	SDL_UnlockTexture(texture);

	pixels = malloc(height * pitch);
	if(!pixels)
	{
		SDL_DestroyTexture(texture);
		return -1;
	}

	surface->width = width;
	surface->height = height;
	surface->pitch = pitch;
	surface->pixels = pixels;
	surface->texture = texture;

	return 0;
}

int sandbox_linux_sdl_fb_surface_destroy(struct sandbox_fb_surface_t * surface)
{
	if(surface)
	{
		SDL_DestroyTexture(surface->texture);
		free(surface->pixels);
	}
	return 0;
}

int sandbox_linux_sdl_fb_surface_present(struct sandbox_fb_surface_t * surface)
{
	SDL_UpdateTexture(surface->texture, NULL, surface->pixels, surface->pitch);
	SDL_RenderCopy(__fb.render, surface->texture, NULL, NULL);
	SDL_RenderPresent(__fb.render);
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
