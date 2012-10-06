#include <system.h>
#include <sandbox/display.h>

static void display_lock(struct display_t * display)
{
	SDL_Surface * screen = (SDL_Surface *)(display->data);
	SDL_LockSurface(screen);
}

static void display_unlock(struct display_t * display)
{
	SDL_Surface * screen = (SDL_Surface *)(display->data);
	SDL_UnlockSurface(screen);
}

static void display_flip(struct display_t * display)
{
	SDL_Surface * screen = (SDL_Surface *)(display->data);
	SDL_Flip(screen);
}

struct display_t * display_alloc(void)
{
	struct system_data_t * d = get_system_data();
	struct display_t * display = NULL;
	const SDL_VideoInfo * info = NULL;
	int width = 800, height = 600;

	if(!d)
		return NULL;

	display = malloc(sizeof(struct display_t));
	if(!display)
		return NULL;

	d->screen = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if(!d->screen)
	{
		free(display);
		return NULL;
	}

	SDL_FillRect(d->screen, NULL, 0);
	SDL_Flip(d->screen);

	info = SDL_GetVideoInfo();
    if(!info)
    {
		free(display);
		return NULL;
    }

	display->width = width;
	display->height = height;
	display->bits_per_pixel = info->vfmt->BitsPerPixel;
	display->bytes_per_pixel = info->vfmt->BytesPerPixel;
	display->red_mask_size = 8;
	display->red_field_pos = info->vfmt->Rshift;
	display->green_mask_size = 8;
	display->green_field_pos = info->vfmt->Gshift;
	display->blue_mask_size = 8;
	display->blue_field_pos = info->vfmt->Bshift;
	display->alpha_mask_size = 8;
	display->alpha_field_pos = 24;

	display->pixels = d->screen->pixels;
	display->lock = display_lock;
	display->unlock = display_unlock;
	display->flip = display_flip;
	display->data = d->screen;

	return display;
}

void display_free(struct display_t * display)
{
	if(display)
		free(display);
}
