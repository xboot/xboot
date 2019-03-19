#include <x.h>
#include <sandbox.h>

struct sandbox_fb_t {
	SDL_Window * window;
	SDL_Surface * screen;
	char * title;
	int width;
	int height;
};

void * sandbox_fb_sdl_open(const char * title, int width, int height)
{
	struct sandbox_fb_t * hdl;
	Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;

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

void sandbox_fb_sdl_close(void * handle)
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

int sandbox_fb_sdl_get_width(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	if(hdl)
		return hdl->width;
	return 0;
}

int sandbox_fb_sdl_get_height(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	if(hdl)
		return hdl->height;
	return 0;
}

int sandbox_fb_sdl_get_pwidth(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	if(hdl)
		return 216;
	return 0;
}

int sandbox_fb_sdl_get_pheight(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	if(hdl)
		return 135;
	return 0;
}

int sandbox_fb_sdl_get_bpp(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	if(hdl)
		return 32;
	return 0;
}

int sandbox_fb_sdl_surface_create(void * handle, struct sandbox_fb_surface_t * surface)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	SDL_Surface * face;
	Uint32 r, g, b, a;
	int bpp;

	SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888, &bpp, &r, &g, &b, &a);
	face = SDL_CreateRGBSurface(SDL_SWSURFACE, hdl->width, hdl->height, bpp, r, g, b, a);
	if(!face)
		return 0;

	surface->width = face->w;
	surface->height = face->h;
	surface->pitch = face->pitch;
	surface->pixels = face->pixels;
	surface->priv = face;

	return 1;
}

int sandbox_fb_sdl_surface_destroy(void * handle, struct sandbox_fb_surface_t * surface)
{
	if(surface)
		SDL_FreeSurface(surface->priv);
	return 1;
}

int sandbox_fb_sdl_surface_present(void * handle, struct sandbox_fb_surface_t * surface, struct sandbox_fb_dirty_rect_t * rect, int nrect)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	SDL_Rect r;
	int i;

	if(rect && (nrect > 0))
	{
		for(i = 0; i < nrect; i++)
		{
			r.x = rect[i].x;
			r.y = rect[i].y;
			r.w = rect[i].w;
			r.h = rect[i].h;
			SDL_BlitSurface(surface->priv, &r, hdl->screen, &r);
		}
	}
	else
	{
		SDL_BlitSurface(surface->priv, NULL, hdl->screen, NULL);
	}
	SDL_UpdateWindowSurface(hdl->window);
	return 1;
}

void sandbox_fb_sdl_set_backlight(void * handle, int brightness)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	SDL_SetWindowBrightness(hdl->window, brightness / 1000.0);
}

int sandbox_fb_sdl_get_backlight(void * handle)
{
	struct sandbox_fb_t * hdl = (struct sandbox_fb_t *)handle;
	return (int)(SDL_GetWindowBrightness(hdl->window) * 1000.0);
}
