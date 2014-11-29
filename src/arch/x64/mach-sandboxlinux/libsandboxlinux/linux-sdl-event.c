#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandboxlinux.h>

struct event_callback_t {
	struct {
		void * device;
		void (*down)(void * device, unsigned int key);
		void (*up)(void * device, unsigned int key);
	} key;

	struct {
		void * device;
		void (*down)(void * device, int x, int y, unsigned int btn);
		void (*move)(void * device, int x, int y);
		void (*up)(void * device, int x, int y, unsigned int btn);
		void (*wheel)(void * device, int dx, int dy);
	} mouse;

	struct {
		void * device;
		void (*begin)(void * device, int x, int y, unsigned int id);
		void (*move)(void * device, int x, int y, unsigned int id);
		void (*end)(void * device, int x, int y, unsigned int id);
	} touch;
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

	.touch = {
		.device		= NULL,
		.begin		= NULL,
		.move		= NULL,
		.end		= NULL,
	},
};
static SDL_Thread * __event = NULL;

static int handle_event(void * data)
{
	struct sandbox_config_t * cfg = sandbox_linux_get_config();
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
				break;

	        case SDL_MOUSEWHEEL:
	        	if(cb->mouse.wheel)
	        		cb->mouse.wheel(cb->mouse.device, e.wheel.x, e.wheel.y);
	        	break;

	        case SDL_FINGERDOWN:
				if(cb->touch.begin)
					cb->touch.begin(cb->touch.device, (int)(e.tfinger.x * cfg->framebuffer.width), (int)(e.tfinger.y * cfg->framebuffer.height), (unsigned int)e.tfinger.fingerId);
	        	break;

	        case SDL_FINGERMOTION:
				if(cb->touch.move)
					cb->touch.move(cb->touch.device, (int)(e.tfinger.x * cfg->framebuffer.width), (int)(e.tfinger.y * cfg->framebuffer.height), (unsigned int)e.tfinger.fingerId);
	        	break;

	        case SDL_FINGERUP:
				if(cb->touch.end)
					cb->touch.end(cb->touch.device, (int)(e.tfinger.x * cfg->framebuffer.width), (int)(e.tfinger.y * cfg->framebuffer.height), (unsigned int)e.tfinger.fingerId);
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
	__event = SDL_CreateThread(handle_event, "event", &__event_callback);
}

void sandbox_linux_sdl_event_exit(void)
{
	if(__event)
		SDL_WaitThread(__event, NULL);
}

void sandbox_linux_sdl_event_set_key_callback(void * device,
		void (*down)(void * device, unsigned int key),
		void (*up)(void * device, unsigned int key))
{
	__event_callback.key.device = device;
	__event_callback.key.down = down;
	__event_callback.key.up = up;
}

void sandbox_linux_sdl_event_set_mouse_callback(void * device,
		void (*down)(void * device, int x, int y, unsigned int btn),
		void (*move)(void * device, int x, int y),
		void (*up)(void * device, int x, int y, unsigned int btn),
		void (*wheel)(void * device, int dx, int dy))
{
	__event_callback.mouse.device = device;
	__event_callback.mouse.down = down;
	__event_callback.mouse.move = move;
	__event_callback.mouse.up = up;
	__event_callback.mouse.wheel = wheel;
}

void sandbox_linux_sdl_event_set_touch_callback(void * device,
		void (*begin)(void * device, int x, int y, unsigned int id),
		void (*move)(void * device, int x, int y, unsigned int id),
		void (*end)(void * device, int x, int y, unsigned int id))
{
	__event_callback.touch.device = device;
	__event_callback.touch.begin = begin;
	__event_callback.touch.move = move;
	__event_callback.touch.end = end;
}
