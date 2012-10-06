#include <system.h>

static struct system_data_t __system_data;

struct system_data_t * get_system_data(void)
{
	return &__system_data;
}

void system_init(void)
{
	struct system_data_t * d = get_system_data();
	
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		return;
	
	d->screen = NULL;
	d->thread = NULL;
	d->quit = 0;
}

void system_exit(void)
{
	SDL_Quit();
}
