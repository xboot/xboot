#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandbox.h>

struct event_callback_t {
	struct {
		void * device;
		void (*down)(void * device, unsigned int key);
		void (*up)(void * device, unsigned int key);
	} key;

	struct {
		void * device;
		void (*down)(void * device, int x, int y, unsigned int button);
		void (*move)(void * device, int x, int y);
		void (*up)(void * device, int x, int y, unsigned int button);
		void (*wheel)(void * device, int dx, int dy);
	} mouse;

	struct {
		void * device;
		void (*begin)(void * device, int x, int y, unsigned int id);
		void (*move)(void * device, int x, int y, unsigned int id);
		void (*end)(void * device, int x, int y, unsigned int id);
	} touch;

	struct {
		void * device;
		void (*left_stick)(void * device, int x, int y);
		void (*right_stick)(void * device, int x, int y);
		void (*left_trigger)(void * device, int v);
		void (*right_trigger)(void * device, int v);
		void (*button_down)(void * device, unsigned int button);
		void (*button_up)(void * device, unsigned int button);
	} joystick;
};

static struct event_callback_t __event_callback = { 0 };
static SDL_Thread * __event = NULL;

static unsigned int keycode_map(SDL_Keycode code)
{
	unsigned int key;

	switch(code)
	{
	case SDLK_LCTRL:
		key = 0x80;
		break;
	case SDLK_RCTRL:
		key = 0x81;
		break;
	case SDLK_LALT:
		key = 0x82;
		break;
	case SDLK_RALT:
		key = 0x83;
		break;
	case SDLK_LSHIFT:
		key = 0x84;
		break;
	case SDLK_RSHIFT:
		key = 0x85;
		break;
	case SDLK_CAPSLOCK:
		key = 0x86;
		break;
	case SDLK_NUMLOCKCLEAR:
		key = 0x87;
		break;
	case SDLK_SCROLLLOCK:
		key = 0x88;
		break;
	case SDLK_DELETE:
		key = 0x90;
		break;
	case SDLK_TAB:
		key = 0x91;
		break;
	case SDLK_RETURN:
		key = 0x92;
		break;
	case SDLK_UP:
		key = 0x93;
		break;
	case SDLK_DOWN:
		key = 0x94;
		break;
	case SDLK_LEFT:
		key = 0x95;
		break;
	case SDLK_RIGHT:
		key = 0x96;
		break;
	case SDLK_PAGEUP:
		key = 0x97;
		break;
	case SDLK_PAGEDOWN:
		key = 0x98;
		break;
	case SDLK_HOME:
		key = 0x99;
		break;
	case SDLK_END:
		key = 0x9a;
		break;
	case SDLK_VOLUMEUP:
		key = 0x9b;
		break;
	case SDLK_VOLUMEDOWN:
		key = 0x9c;
		break;
	case SDLK_MENU:
		key = 0x9d;
		break;
	case SDLK_ESCAPE:
		key = 0x9e;
		break;
	case SDLK_POWER:
		key = 0x9f;
		break;
	default:
		key = code;
	}
	return key;
}

static int handle_event(void * data)
{
	struct event_callback_t * cb = (struct event_callback_t *)(data);
	SDL_GameController * gc = NULL;
	SDL_Event e;
	int x, y, v;
	unsigned int button;

	while(1)
	{
		if(SDL_WaitEvent(&e))
		{
	        switch(e.type)
	        {
	        case SDL_KEYDOWN:
	        	if(cb->key.down)
	        		cb->key.down(cb->key.device, keycode_map(e.key.keysym.sym));
	        	break;

	        case SDL_KEYUP:
	        	if(cb->key.up)
	        		cb->key.up(cb->key.device, keycode_map(e.key.keysym.sym));
	        	break;

	        case SDL_MOUSEBUTTONDOWN:
	        	switch(e.button.button)
	        	{
				case SDL_BUTTON_LEFT:
					button = 0x01;
					break;
				case SDL_BUTTON_MIDDLE:
					button = 0x02;
					break;
				case SDL_BUTTON_RIGHT:
					button = 0x03;
					break;
				case SDL_BUTTON_X1:
					button = 0x04;
					break;
				case SDL_BUTTON_X2:
					button = 0x05;
					break;
				default:
					button = 0x00;
					break;
	        	}
				if(cb->mouse.down && (button != 0x00))
					cb->mouse.down(cb->mouse.device, e.button.x, e.button.y, button);
				break;

	        case SDL_MOUSEMOTION:
	        	if(cb->mouse.move)
	        		cb->mouse.move(cb->mouse.device, e.motion.x, e.motion.y);
	        	break;

	        case SDL_MOUSEBUTTONUP:
	        	switch(e.button.button)
	        	{
				case SDL_BUTTON_LEFT:
					button = 0x01;
					break;
				case SDL_BUTTON_MIDDLE:
					button = 0x02;
					break;
				case SDL_BUTTON_RIGHT:
					button = 0x03;
					break;
				case SDL_BUTTON_X1:
					button = 0x04;
					break;
				case SDL_BUTTON_X2:
					button = 0x05;
					break;
				default:
					button = 0x00;
					break;
	        	}
				if(cb->mouse.up && (button != 0x00))
					cb->mouse.up(cb->mouse.device, e.button.x, e.button.y, button);
				break;

	        case SDL_MOUSEWHEEL:
	        	if(cb->mouse.wheel)
	        		cb->mouse.wheel(cb->mouse.device, e.wheel.x, e.wheel.y);
	        	break;
/*
	        case SDL_FINGERDOWN:
				if(cb->touch.begin)
					cb->touch.begin(cb->touch.device, (int)(e.tfinger.x * sandbox_sdl_fb_get_width()), (int)(e.tfinger.y * sandbox_sdl_fb_get_height()), (unsigned int)e.tfinger.fingerId);
	        	break;

	        case SDL_FINGERMOTION:
				if(cb->touch.move)
					cb->touch.move(cb->touch.device, (int)(e.tfinger.x * sandbox_sdl_fb_get_width()), (int)(e.tfinger.y * sandbox_sdl_fb_get_height()), (unsigned int)e.tfinger.fingerId);
	        	break;

	        case SDL_FINGERUP:
				if(cb->touch.end)
					cb->touch.end(cb->touch.device, (int)(e.tfinger.x * sandbox_sdl_fb_get_width()), (int)(e.tfinger.y * sandbox_sdl_fb_get_height()), (unsigned int)e.tfinger.fingerId);
	        	break;
*/
	        case SDL_CONTROLLERDEVICEADDED:
	        	if(!gc)
	        		gc = SDL_GameControllerOpen(e.cdevice.which);
	            break;

	        case SDL_CONTROLLERDEVICEREMOVED:
	        	if(gc)
	        	{
	        		SDL_GameControllerClose(gc);
	        		gc = NULL;
	        	}
	            break;

	        case SDL_CONTROLLERAXISMOTION:
	        	switch(e.caxis.axis)
	        	{
				case SDL_CONTROLLER_AXIS_LEFTX:
				case SDL_CONTROLLER_AXIS_LEFTY:
					x = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTX);
					y = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);
					if(cb->joystick.left_stick)
						cb->joystick.left_stick(cb->joystick.device, x, y);
					break;

				case SDL_CONTROLLER_AXIS_RIGHTX:
				case SDL_CONTROLLER_AXIS_RIGHTY:
					x = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_RIGHTX);
					y = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_RIGHTY);
					if(cb->joystick.right_stick)
						cb->joystick.right_stick(cb->joystick.device, x, y);
					break;

				case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
					v = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
					if(cb->joystick.left_trigger)
						cb->joystick.left_trigger(cb->joystick.device, v);
					break;

				case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
					v = SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
					if(cb->joystick.right_trigger)
						cb->joystick.right_trigger(cb->joystick.device, v);
					break;

				default:
					break;
	        	}
	        	break;

	        case SDL_CONTROLLERBUTTONDOWN:
				switch(e.cbutton.button)
				{
				case SDL_CONTROLLER_BUTTON_A:
					button = 0x05;
					break;
				case SDL_CONTROLLER_BUTTON_B:
					button = 0x06;
					break;
				case SDL_CONTROLLER_BUTTON_X:
					button = 0x07;
					break;
				case SDL_CONTROLLER_BUTTON_Y:
					button = 0x08;
					break;
				case SDL_CONTROLLER_BUTTON_BACK:
					button = 0x09;
					break;
				case SDL_CONTROLLER_BUTTON_GUIDE:
					button = 0x0b;
					break;
				case SDL_CONTROLLER_BUTTON_START:
					button = 0x0a;
					break;
				case SDL_CONTROLLER_BUTTON_LEFTSTICK:
					button = 0x0e;
					break;
				case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
					button = 0x0f;
					break;
				case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
					button = 0x0c;
					break;
				case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
					button = 0x0d;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_UP:
					button = 0x01;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
					button = 0x02;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
					button = 0x03;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
					button = 0x04;
					break;
				default:
					button = 0x00;
					break;
				}
				if(cb->joystick.button_down && (button != 0x00))
					cb->joystick.button_down(cb->joystick.device, button);
				break;

	        case SDL_CONTROLLERBUTTONUP:
				switch(e.cbutton.button)
				{
				case SDL_CONTROLLER_BUTTON_A:
					button = 0x05;
					break;
				case SDL_CONTROLLER_BUTTON_B:
					button = 0x06;
					break;
				case SDL_CONTROLLER_BUTTON_X:
					button = 0x07;
					break;
				case SDL_CONTROLLER_BUTTON_Y:
					button = 0x08;
					break;
				case SDL_CONTROLLER_BUTTON_BACK:
					button = 0x09;
					break;
				case SDL_CONTROLLER_BUTTON_GUIDE:
					button = 0x0b;
					break;
				case SDL_CONTROLLER_BUTTON_START:
					button = 0x0a;
					break;
				case SDL_CONTROLLER_BUTTON_LEFTSTICK:
					button = 0x0e;
					break;
				case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
					button = 0x0f;
					break;
				case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
					button = 0x0c;
					break;
				case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
					button = 0x0d;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_UP:
					button = 0x01;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
					button = 0x02;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
					button = 0x03;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
					button = 0x04;
					break;
				default:
					button = 0x00;
					break;
				}
				if(cb->joystick.button_up && (button != 0x00))
					cb->joystick.button_up(cb->joystick.device, button);
	        	break;

	        case SDL_QUIT:
	        	sandbox_exit();
	        	break;

	        default:
	        	break;
	        }
		}
	}

	return 0;
}

void sandbox_sdl_event_init(void)
{
	memset(&__event_callback, 0, sizeof(struct event_callback_t));
	__event = SDL_CreateThread(handle_event, "event", &__event_callback);
}

void sandbox_sdl_event_exit(void)
{
	if(__event)
		SDL_WaitThread(__event, NULL);
}

void sandbox_sdl_event_set_key_callback(void * device,
		void (*down)(void * device, unsigned int key),
		void (*up)(void * device, unsigned int key))
{
	__event_callback.key.device = device;
	__event_callback.key.down = down;
	__event_callback.key.up = up;
}

void sandbox_sdl_event_set_mouse_callback(void * device,
		void (*down)(void * device, int x, int y, unsigned int button),
		void (*move)(void * device, int x, int y),
		void (*up)(void * device, int x, int y, unsigned int button),
		void (*wheel)(void * device, int dx, int dy))
{
	__event_callback.mouse.device = device;
	__event_callback.mouse.down = down;
	__event_callback.mouse.move = move;
	__event_callback.mouse.up = up;
	__event_callback.mouse.wheel = wheel;
}

void sandbox_sdl_event_set_touch_callback(void * device,
		void (*begin)(void * device, int x, int y, unsigned int id),
		void (*move)(void * device, int x, int y, unsigned int id),
		void (*end)(void * device, int x, int y, unsigned int id))
{
	__event_callback.touch.device = device;
	__event_callback.touch.begin = begin;
	__event_callback.touch.move = move;
	__event_callback.touch.end = end;
}

void sandbox_sdl_event_set_joystick_callback(void * device,
		void (*left_stick)(void * device, int x, int y),
		void (*right_stick)(void * device, int x, int y),
		void (*left_trigger)(void * device, int v),
		void (*right_trigger)(void * device, int v),
		void (*button_down)(void * device, unsigned int button),
		void (*button_up)(void * device, unsigned int button))
{
	__event_callback.joystick.device = device;
	__event_callback.joystick.left_stick = left_stick;
	__event_callback.joystick.right_stick = right_stick;
	__event_callback.joystick.left_trigger = left_trigger;
	__event_callback.joystick.right_trigger = right_trigger;
	__event_callback.joystick.button_down = button_down;
	__event_callback.joystick.button_up = button_up;
}
