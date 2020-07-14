#include <x.h>
#include <sandbox.h>

struct sandbox_fb_sdl_context_t {
	SDL_Window * window;
	SDL_Surface * screen;
	char * title;
	int width;
	int height;
};

void * sandbox_fb_sdl_open(const char * title, int width, int height)
{
	struct sandbox_fb_sdl_context_t * ctx;
	Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS;

	ctx = malloc(sizeof(struct sandbox_fb_sdl_context_t));
	if(!ctx)
		return NULL;

	ctx->title = strdup(title ? title : "Xboot Runtime Environment");
	ctx->window = SDL_CreateWindow(ctx->title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	if(!ctx->window)
	{
		if(ctx->title)
			free(ctx->title);
		free(ctx);
		return NULL;
	}

	ctx->screen = SDL_GetWindowSurface(ctx->window);
	if(!ctx->screen)
	{
		if(ctx->window)
			SDL_DestroyWindow(ctx->window);
		if(ctx->title)
			free(ctx->title);
		free(ctx);
		return NULL;
	}

	SDL_GetWindowSize(ctx->window, &ctx->width, &ctx->height);
	return ctx;
}

void sandbox_fb_sdl_close(void * context)
{
	struct sandbox_fb_sdl_context_t * ctx = (struct sandbox_fb_sdl_context_t *)context;

	if(!ctx)
		return;
	if(ctx->window)
		SDL_DestroyWindow(ctx->window);
	if(ctx->screen)
		SDL_FreeSurface(ctx->screen);
	if(ctx->title)
		free(ctx->title);
	free(ctx);
}

int sandbox_fb_sdl_get_width(void * context)
{
	struct sandbox_fb_sdl_context_t * ctx = (struct sandbox_fb_sdl_context_t *)context;
	if(ctx)
		return ctx->width;
	return 0;
}

int sandbox_fb_sdl_get_height(void * context)
{
	struct sandbox_fb_sdl_context_t * ctx = (struct sandbox_fb_sdl_context_t *)context;
	if(ctx)
		return ctx->height;
	return 0;
}

int sandbox_fb_sdl_get_pwidth(void * context)
{
	struct sandbox_fb_sdl_context_t * ctx = (struct sandbox_fb_sdl_context_t *)context;
	if(ctx)
		return 216;
	return 0;
}

int sandbox_fb_sdl_get_pheight(void * context)
{
	struct sandbox_fb_sdl_context_t * ctx = (struct sandbox_fb_sdl_context_t *)context;
	if(ctx)
		return 135;
	return 0;
}

int sandbox_fb_sdl_surface_create(void * context, struct sandbox_fb_surface_t * surface)
{
	struct sandbox_fb_sdl_context_t * ctx = (struct sandbox_fb_sdl_context_t *)context;
	SDL_Surface * face;
	Uint32 r, g, b, a;
	int bpp;

	SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888, &bpp, &r, &g, &b, &a);
	face = SDL_CreateRGBSurface(SDL_SWSURFACE, ctx->width, ctx->height, bpp, r, g, b, a);
	if(!face)
		return 0;

	surface->width = face->w;
	surface->height = face->h;
	surface->stride = face->pitch;
	surface->pixlen = face->h * face->pitch;
	surface->pixels = face->pixels;
	surface->priv = face;

	return 1;
}

int sandbox_fb_sdl_surface_destroy(void * context, struct sandbox_fb_surface_t * surface)
{
	if(surface)
		SDL_FreeSurface(surface->priv);
	return 1;
}

int sandbox_fb_sdl_surface_present(void * context, struct sandbox_fb_surface_t * surface, struct sandbox_region_list_t * rl)
{
	struct sandbox_fb_sdl_context_t * ctx = (struct sandbox_fb_sdl_context_t *)context;
	struct sandbox_region_t * r;
	int i;

	if(rl && (rl->count > 0))
	{
		for(i = 0; i < rl->count; i++)
		{
			r = &rl->region[i];
			SDL_BlitSurface(surface->priv, (SDL_Rect *)r, ctx->screen, (SDL_Rect *)r);
		}
	}
	else
	{
		SDL_BlitSurface(surface->priv, NULL, ctx->screen, NULL);
	}
	SDL_UpdateWindowSurface(ctx->window);
	return 1;
}

void sandbox_fb_sdl_set_backlight(void * context, int brightness)
{
	struct sandbox_fb_sdl_context_t * ctx = (struct sandbox_fb_sdl_context_t *)context;
	SDL_SetWindowBrightness(ctx->window, brightness / 1000.0);
}

int sandbox_fb_sdl_get_backlight(void * context)
{
	struct sandbox_fb_sdl_context_t * ctx = (struct sandbox_fb_sdl_context_t *)context;
	return (int)(SDL_GetWindowBrightness(ctx->window) * 1000.0);
}
