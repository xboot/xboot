#include <system.h>
#include <sandbox/input.h>

static int thread_function(void * data)
{
	static unsigned long start_time = 0;
	long delay_time;
	struct system_data_t * d = get_system_data();
	SDL_Event ev;

	if(!d)
		return -1;

	while(d->quit == 0)
	{
		while(SDL_PollEvent(&ev))
		{
			switch(ev.type)
			{
			case SDL_KEYDOWN:
				//sandbox_keyboard_updown(ev.key.keysym.sym, 1);
				break;
			case SDL_KEYUP:
				//sandbox_keyboard_updown(ev.key.keysym.sym, 0);
				break;
			case SDL_MOUSEMOTION:
				break;
			case SDL_MOUSEBUTTONDOWN:
				break;
			case SDL_MOUSEBUTTONUP:
				break;
			case SDL_QUIT:
				d->quit = 1;
				SDL_Quit();
				break;
			default:
				break;
			}
		}

		SDL_Flip(d->screen);

		delay_time = (1000 / 60) - (SDL_GetTicks() - start_time);
		if(delay_time > 0)
			SDL_Delay(delay_time);
		start_time = SDL_GetTicks();
	}

	return 0;
}

struct input_t * input_alloc(void)
{
	struct system_data_t * d = get_system_data();
	struct input_t * input;
	
	if(!d)
		return NULL;

	input = malloc(sizeof(struct input_t));
	if(!input)
		return NULL;
	
	d->thread = SDL_CreateThread(thread_function, d);
	if(!d->thread)
	{
		free(input);
		return NULL;
	}

	input->data = d->thread;

	return input;
}

void input_free(struct input_t * input)
{
	if(!input)
		return;

	if(input->data)
		SDL_KillThread((SDL_Thread *)(input->data));

	free(input);
}
