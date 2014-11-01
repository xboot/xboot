#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandboxlinux.h>

struct event_callback_t {
	struct {
		void * device;
		void (*down)(void * device, unsigned int code);
		void (*up)(void * device, unsigned int code);
	} key;

	struct {
		void * device;
		void (*down)(void * device, int x, int y, unsigned int btn);
		void (*move)(void * device, int x, int y);
		void (*up)(void * device, int x, int y, unsigned int btn);
		void (*wheel)(void * device, int x, int y, int delta);
	} mouse;
};

static struct event_callback_t __event_callback = {
	.key = {
		.device		= NULL,
		.down		= NULL,
		.up			= NULL,
	},

	.mouse = {
		.device		= NULL,
		.down		= NULL,
		.move		= NULL,
		.up			= NULL,
		.wheel		= NULL,
	},
};
static SDL_Thread * __event_thread = NULL;

static int handle_event(void * data)
{
	struct event_callback_t * cb = (struct event_callback_t *)(data);
	SDL_Event e;

	while(1)
	{
		if(SDL_WaitEvent(&e))
		{
	        switch(e.type)
	        {
	        case SDL_KEYDOWN:
	        	if(cb->key.down)
	        		cb->key.down(cb->key.device, e.key.keysym.sym);
	        	break;

	        case SDL_KEYUP:
	        	if(cb->key.up)
	        		cb->key.up(cb->key.device, e.key.keysym.sym);
	        	break;

	        case SDL_MOUSEBUTTONDOWN:
				if(e.button.button == SDL_BUTTON_LEFT)
				{
					if(cb->mouse.down)
						cb->mouse.down(cb->mouse.device, e.button.x, e.button.y, (0x1 << 0));
				}
				else if(e.button.button == SDL_BUTTON_RIGHT)
				{
					if(cb->mouse.down)
						cb->mouse.down(cb->mouse.device, e.button.x, e.button.y, (0x1 << 1));
				}
				else if(e.button.button == SDL_BUTTON_MIDDLE)
				{
					if(cb->mouse.down)
						cb->mouse.down(cb->mouse.device, e.button.x, e.button.y, (0x1 << 2));
				}
				else if(e.button.button == SDL_BUTTON_WHEELUP)
				{
					if(cb->mouse.wheel)
						cb->mouse.wheel(cb->mouse.device, e.button.x, e.button.y, -1);
				}
				else if(e.button.button == SDL_BUTTON_WHEELDOWN)
				{
					if(cb->mouse.wheel)
						cb->mouse.wheel(cb->mouse.device, e.button.x, e.button.y, 1);
				}
				break;

	        case SDL_MOUSEMOTION:
	        	if(cb->mouse.move)
	        		cb->mouse.move(cb->mouse.device, e.motion.x, e.motion.y);
	        	break;

	        case SDL_MOUSEBUTTONUP:
			if(e.button.button == SDL_BUTTON_LEFT)
			{
				if(cb->mouse.up)
					cb->mouse.up(cb->mouse.device, e.button.x, e.button.y, (0x1 << 0));
			}
			else if(e.button.button == SDL_BUTTON_RIGHT)
			{
				if(cb->mouse.up)
					cb->mouse.up(cb->mouse.device, e.button.x, e.button.y, (0x1 << 1));
			}
			else if(e.button.button == SDL_BUTTON_MIDDLE)
			{
				if(cb->mouse.up)
					cb->mouse.up(cb->mouse.device, e.button.x, e.button.y, (0x1 << 2));
			}
			else if(e.button.button == SDL_BUTTON_WHEELUP)
			{
				if(cb->mouse.wheel)
					cb->mouse.wheel(cb->mouse.device, e.button.x, e.button.y, -1);
			}
			else if(e.button.button == SDL_BUTTON_WHEELDOWN)
			{
				if(cb->mouse.wheel)
					cb->mouse.wheel(cb->mouse.device, e.button.x, e.button.y, 1);
			}
			break;

	        case SDL_QUIT:
	        	sandbox_linux_exit();
	        	break;

	        default:
	        	break;
	        }
		}
	}

	return 0;
}

void sandbox_linux_sdl_event_init(void)
{
	__event_thread = SDL_CreateThread(handle_event, &__event_callback);
}

void sandbox_linux_sdl_event_exit(void)
{
	if(__event_thread)
		SDL_WaitThread(__event_thread, NULL);
}

void sandbox_linux_sdl_event_set_key_callback(void * device,
		void (*down)(void * device, unsigned int code),
		void (*up)(void * device, unsigned int code))
{
	__event_callback.key.device = device;
	__event_callback.key.down = down;
	__event_callback.key.up = up;
}

void sandbox_linux_sdl_event_set_mouse_callback(void * device,
		void (*down)(void * device, int x, int y, unsigned int btn),
		void (*move)(void * device, int x, int y),
		void (*up)(void * device, int x, int y, unsigned int btn),
		void (*wheel)(void * device, int x, int y, int delta))
{
	__event_callback.mouse.device = device;
	__event_callback.mouse.down = down;
	__event_callback.mouse.move = move;
	__event_callback.mouse.up = up;
	__event_callback.mouse.wheel = wheel;
}
