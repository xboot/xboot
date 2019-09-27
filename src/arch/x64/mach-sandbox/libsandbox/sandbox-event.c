#include <x.h>
#include <sandbox.h>

struct sandbox_event_callback_t {
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

struct sandbox_event_buffer_t {
	struct {
		int code;
		int value;
	} keyboard;
	struct {
		int relx;
		int rely;
		int deltax;
		int deltay;
		int btndown;
		int btnup;
	} mouse;
	struct {
		int x;
		int y;
		int press;
		int move;
	} stouch;
	struct {
		int x;
		int y;
	} mtouch;
};

struct sandbox_event_context_t {
	struct sandbox_event_callback_t cb;
	struct pollfd * pfd;
	int npfd;
	char ** device;
	struct sandbox_event_buffer_t * buf;
	pthread_t thread;
	int xmax, ymax;
	int sensitivity;
	int xpos, ypos;
};

static int sandbox_event_open_device(struct sandbox_event_context_t * ctx, const char * dev)
{
	struct pollfd * new_pfd;
	struct input_id id;
	char ** new_device;
	struct sandbox_event_buffer_t * new_buf;
	char name[80];
	char location[80];
	char idstr[80];
	int clkid = CLOCK_MONOTONIC;
	int version;
	int fd;

	fd = open(dev, O_RDWR);
	if(fd < 0)
		return -1;
	if(ioctl(fd, EVIOCGVERSION, &version))
		return -1;
	if(ioctl(fd, EVIOCGID, &id))
		return -1;
	ioctl(fd, EVIOCSCLOCKID, &clkid);

	name[sizeof(name) - 1] = '\0';
	location[sizeof(location) - 1] = '\0';
	idstr[sizeof(idstr) - 1] = '\0';
	if(ioctl(fd, EVIOCGNAME(sizeof(name) - 1), &name) < 1)
		name[0] = '\0';
	if(ioctl(fd, EVIOCGPHYS(sizeof(location) - 1), &location) < 1)
		location[0] = '\0';
	if(ioctl(fd, EVIOCGUNIQ(sizeof(idstr) - 1), &idstr) < 1)
		idstr[0] = '\0';

	new_pfd = realloc(ctx->pfd, sizeof(struct pollfd) * (ctx->npfd + 1));
	if(!new_pfd)
		return -1;
	ctx->pfd = new_pfd;

	new_device = realloc(ctx->device, sizeof(ctx->device[0]) * (ctx->npfd + 1));
	if(new_device == NULL)
		return -1;
	ctx->device = new_device;

	new_buf = realloc(ctx->buf, sizeof(struct sandbox_event_buffer_t) * (ctx->npfd + 1));
	if(new_buf == NULL)
		return -1;
	ctx->buf = new_buf;
	memset(&ctx->buf[ctx->npfd], 0, sizeof(struct sandbox_event_buffer_t));

	ctx->pfd[ctx->npfd].fd = fd;
	ctx->pfd[ctx->npfd].events = POLLIN;
	ctx->device[ctx->npfd] = strdup(dev);
	ctx->npfd++;
	return 0;
}

static int sandbox_event_close_device(struct sandbox_event_context_t * ctx, const char * dev)
{
	int i;
	for(i = 1; i < ctx->npfd; i++)
	{
		if(strcmp(ctx->device[i], dev) == 0)
		{
			int count = ctx->npfd - i - 1;
			free(ctx->device[i]);
			memmove(ctx->buf + i, ctx->buf + i + 1, sizeof(struct sandbox_event_buffer_t) * count);
			memmove(ctx->device + i, ctx->device + i + 1, sizeof(ctx->device[0]) * count);
			memmove(ctx->pfd + i, ctx->pfd + i + 1, sizeof(struct pollfd) * count);
			ctx->npfd--;
			return 0;
		}
	}
	return -1;
}

static int sandbox_event_read_notify(struct sandbox_event_context_t * ctx, const char * dirname, int nfd)
{
	struct inotify_event * event;
	char dev[PATH_MAX];
	char * name;
	char buf[512];
	int pos = 0;
	int size;
	int res;

	res = read(nfd, buf, sizeof(buf));
	if(res < sizeof(struct inotify_event))
	{
		if(errno == EINTR)
			return 0;
		return 1;
	}

	strcpy(dev, dirname);
	name = dev + strlen(dev);
	*name++ = '/';

	while(res >= sizeof(struct inotify_event))
	{
		event = (struct inotify_event *)(buf + pos);
		if(event->len)
		{
			strcpy(name, event->name);
			if(event->mask & IN_CREATE)
				sandbox_event_open_device(ctx, dev);
			else
				sandbox_event_close_device(ctx, dev);
		}
		size = sizeof(struct inotify_event) + event->len;
		res -= size;
		pos += size;
	}
	return 0;
}

static int sandbox_event_scan_dir(struct sandbox_event_context_t * ctx, const char * dirname)
{
	struct dirent * de;
	char dev[PATH_MAX];
	char * name;
	DIR * dir;

	dir = opendir(dirname);
	if(dir == NULL)
		return -1;

	strcpy(dev, dirname);
	name = dev + strlen(dev);
	*name++ = '/';

	while((de = readdir(dir)))
	{
		if(de->d_name[0] == '.' && (de->d_name[1] == '\0' || (de->d_name[1] == '.' && de->d_name[2] == '\0')))
			continue;
		strcpy(name, de->d_name);
		sandbox_event_open_device(ctx, dev);
	}
	closedir(dir);
	return 0;
}

static unsigned int keycode_map(int code)
{
	unsigned int key;

	switch(code)
	{
	case 116: key = 1; break;
	case 103: key = 2; break;
	case 108: key = 3; break;
	case 105: key = 4; break;
	case 106: key = 5; break;
	case 115: key = 6; break;
	case 114: key = 7; break;
	case 113: key = 8; break;
	case 102: key = 9;  break;
	case 15:  key = 10; break;
	case 1:   key = 11; break;
	case 14:  key = 12; break;
	case 28:  key = 13; break;
	case 29:  key = 14; break;
	case 97:  key = 15; break;
	case 56:  key = 16; break;
	case 100: key = 17; break;
	case 42:  key = 18; break;
	case 54:  key = 19; break;

	case 59:  key = 20; break;
	case 60:  key = 21; break;
	case 61:  key = 22; break;
	case 62:  key = 23; break;
	case 63:  key = 24; break;
	case 64:  key = 25; break;
	case 65:  key = 26; break;
	case 66:  key = 27; break;
	case 67:  key = 28; break;
	case 68:  key = 29; break;
	case 87:  key = 30; break;
	case 88:  key = 31; break;

	default:
		key = code;
		break;
	}
	return key;
}

static void * sandbox_event_thread(void * arg)
{
	struct sandbox_event_context_t * ctx = (struct sandbox_event_context_t *)arg;
	struct sandbox_event_callback_t * cb = &ctx->cb;
	struct input_event e;
	int i;

	while(1)
	{
		poll(ctx->pfd, ctx->npfd, -1);
		if(ctx->pfd[0].revents & POLLIN)
			sandbox_event_read_notify(ctx, "/dev/input", ctx->pfd[0].fd);

		for(i = 1; i < ctx->npfd; i++)
		{
			if(ctx->pfd[i].revents)
			{
				if(ctx->pfd[i].revents & POLLIN)
				{
					if(read(ctx->pfd[i].fd, &e, sizeof(struct input_event)) == sizeof(struct input_event))
					{
						switch(e.type)
						{
						case EV_SYN:
							if(e.code == SYN_REPORT)
							{
								if(ctx->buf[i].keyboard.code != 0)
								{
									if(ctx->buf[i].keyboard.value == 0)
									{
										if(cb->key.up)
											cb->key.up(cb->key.device, keycode_map(ctx->buf[i].keyboard.code));
									}
									else
									{
										if(cb->key.down)
											cb->key.down(cb->key.device, keycode_map(ctx->buf[i].keyboard.code));
									}
									ctx->buf[i].keyboard.code = 0;
								}

								if((ctx->buf[i].mouse.relx != 0) || (ctx->buf[i].mouse.rely != 0))
								{
									if(ctx->buf[i].mouse.relx != 0)
									{
										ctx->xpos += ctx->buf[i].mouse.relx;
										if(ctx->xpos < 0)
											ctx->xpos = 0;
										if(ctx->xpos > ctx->xmax - 1)
											ctx->xpos = ctx->xmax - 1;
									}
									if(ctx->buf[i].mouse.rely != 0)
									{
										ctx->ypos += ctx->buf[i].mouse.rely;
										if(ctx->ypos < 0)
											ctx->ypos = 0;
										if(ctx->ypos > ctx->ymax - 1)
											ctx->ypos = ctx->ymax - 1;
									}
									if(cb->mouse.move)
										cb->mouse.move(cb->mouse.device, ctx->xpos, ctx->ypos);
									ctx->buf[i].mouse.relx = 0;
									ctx->buf[i].mouse.rely = 0;
								}
								if(ctx->buf[i].mouse.btndown)
								{
									if(cb->mouse.down)
										cb->mouse.down(cb->mouse.device, ctx->xpos, ctx->ypos, ctx->buf[i].mouse.btndown);
									ctx->buf[i].mouse.btndown = 0;
								}
								if(ctx->buf[i].mouse.btnup)
								{
									if(cb->mouse.up)
										cb->mouse.up(cb->mouse.device, ctx->xpos, ctx->ypos, ctx->buf[i].mouse.btnup);
									ctx->buf[i].mouse.btnup = 0;
								}
								if((ctx->buf[i].mouse.deltax != 0) || (ctx->buf[i].mouse.deltay != 0))
								{
									if(cb->mouse.wheel)
										cb->mouse.wheel(cb->mouse.device, ctx->buf[i].mouse.deltax, ctx->buf[i].mouse.deltay);
									ctx->buf[i].mouse.deltax = 0;
									ctx->buf[i].mouse.deltay = 0;
								}

								if(ctx->buf[i].stouch.press == 1)
								{
									if(cb->touch.begin)
										cb->touch.begin(cb->touch.device, ctx->buf[i].stouch.x, ctx->buf[i].stouch.y, 0);
									ctx->buf[i].stouch.press = 0;
								}
								else if(ctx->buf[i].stouch.press == -1)
								{
									if(cb->touch.end)
										cb->touch.end(cb->touch.device, ctx->buf[i].stouch.x, ctx->buf[i].stouch.y, 0);
									ctx->buf[i].stouch.press = 0;
								}
								if(ctx->buf[i].stouch.move != 0)
								{
									if(cb->touch.move)
										cb->touch.move(cb->touch.device, ctx->buf[i].stouch.x, ctx->buf[i].stouch.y, 0);
									ctx->buf[i].stouch.move = 0;
								}
							}
							break;
						case EV_KEY:
							if(e.code >= BTN_MOUSE && e.code < (BTN_MOUSE + 16))
							{
								if(e.value == 0)
									ctx->buf[i].mouse.btnup |= 1 << (e.code - BTN_MOUSE);
								else
									ctx->buf[i].mouse.btndown |= 1 << (e.code - BTN_MOUSE);
							}
							else if(e.code >= BTN_JOYSTICK && e.code < (BTN_JOYSTICK + 16))
							{
							}
							else if(e.code >= BTN_GAMEPAD && e.code < (BTN_GAMEPAD + 16))
							{
							}
							else if(e.code >= BTN_DIGI && e.code < (BTN_DIGI + 16))
							{
								if(e.code == BTN_TOUCH)
									ctx->buf[i].stouch.press = (e.value != 0) ? 1 : -1;
							}
							else if(e.code >= BTN_WHEEL && e.code < (BTN_WHEEL + 16))
							{
							}
							else
							{
								ctx->buf[i].keyboard.code = e.code;
								ctx->buf[i].keyboard.value = e.value;
							}
							break;
						case EV_REL:
							switch(e.code)
							{
							case REL_X:
								ctx->buf[i].mouse.relx = e.value;
								break;
							case REL_Y:
								ctx->buf[i].mouse.rely = e.value;
								break;
							case REL_HWHEEL:
								ctx->buf[i].mouse.deltax = e.value;
								break;
							case REL_WHEEL:
								ctx->buf[i].mouse.deltay = e.value;
								break;
							default:
								break;
							}
							break;
						case EV_ABS:
							switch(e.code)
							{
							case ABS_X:
								ctx->buf[i].stouch.x = e.value;
								ctx->buf[i].stouch.move = 1;
								break;
							case ABS_Y:
								ctx->buf[i].stouch.y = e.value;
								ctx->buf[i].stouch.move = 1;
								break;
							default:
								break;
							}
							break;
						case EV_MSC:
							break;
						case EV_SW:
							break;
						case EV_LED:
							break;
						case EV_SND:
							break;
						case EV_REP:
							break;
						case EV_FF:
							break;
						case EV_PWR:
							break;
						case EV_FF_STATUS:
							break;
						default:
							break;
						}
					}
				}
			}
		}
	}
	return 0;
}

void * sandbox_event_open(void)
{
	struct sandbox_event_context_t * ctx;
	int i;

	ctx = malloc(sizeof(struct sandbox_event_context_t));
	if(!ctx)
		return NULL;

	memset(ctx, 0, sizeof(struct sandbox_event_context_t));
	ctx->npfd = 1;
	ctx->pfd = calloc(ctx->npfd, sizeof(struct pollfd));
	ctx->pfd[0].fd = inotify_init();
	ctx->pfd[0].events = POLLIN;
	ctx->xmax = 640;
	ctx->ymax = 480;
	ctx->sensitivity = 6;
	ctx->xpos = 0;
	ctx->ypos = 0;
	if(inotify_add_watch(ctx->pfd[0].fd, "/dev/input", IN_DELETE | IN_CREATE) < 0)
	{
		free(ctx->pfd);
		free(ctx);
		return NULL;
	}

	if(sandbox_event_scan_dir(ctx, "/dev/input") < 0)
	{
		for(i = 0; i < ctx->npfd; i++)
			inotify_rm_watch(ctx->pfd[i].fd, IN_DELETE | IN_CREATE);
		free(ctx->pfd);
		free(ctx);
		return NULL;
	}

	if(pthread_create(&ctx->thread, NULL, sandbox_event_thread, ctx) < 0)
	{
		for(i = 0; i < ctx->npfd; i++)
			inotify_rm_watch(ctx->pfd[i].fd, IN_DELETE | IN_CREATE);
		free(ctx->pfd);
		free(ctx);
		return NULL;
	}
	return ctx;
}

void sandbox_event_close(void * context)
{
	struct sandbox_event_context_t * ctx = (struct sandbox_event_context_t *)context;
	int i;

	if(ctx)
	{
		for(i = 0; i < ctx->npfd; i++)
			inotify_rm_watch(ctx->pfd[i].fd, IN_DELETE | IN_CREATE);
		free(ctx->pfd);
		free(ctx);
	}
}

void sandbox_event_mouse_set_range(void * context, int xmax, int ymax)
{
	struct sandbox_event_context_t * ctx = (struct sandbox_event_context_t *)context;
	if(xmax > 0)
		ctx->xmax = xmax;
	if(ymax > 0)
		ctx->ymax = ymax;
}

void sandbox_event_mouse_get_range(void * context, int * xmax, int * ymax)
{
	struct sandbox_event_context_t * ctx = (struct sandbox_event_context_t *)context;
	if(xmax)
		*xmax = ctx->xmax;
	if(ymax)
		*ymax = ctx->ymax;
}

void sandbox_event_mouse_set_sensitivity(void * context, int s)
{
	struct sandbox_event_context_t * ctx = (struct sandbox_event_context_t *)context;
	ctx->sensitivity = s;
}

void sandbox_event_mouse_get_sensitivity(void * context, int * s)
{
	struct sandbox_event_context_t * ctx = (struct sandbox_event_context_t *)context;
	if(s)
		*s = ctx->sensitivity;
}

void sandbox_event_set_key_callback(void * context, void * device,
		void (*down)(void * device, unsigned int key),
		void (*up)(void * device, unsigned int key))
{
	struct sandbox_event_context_t * ctx = (struct sandbox_event_context_t *)context;
	if(ctx)
	{
		ctx->cb.key.device = device;
		ctx->cb.key.down = down;
		ctx->cb.key.up = up;
	}
}

void sandbox_event_set_mouse_callback(void * context, void * device,
		void (*down)(void * device, int x, int y, unsigned int button),
		void (*move)(void * device, int x, int y),
		void (*up)(void * device, int x, int y, unsigned int button),
		void (*wheel)(void * device, int dx, int dy))
{
	struct sandbox_event_context_t * ctx = (struct sandbox_event_context_t *)context;
	if(ctx)
	{
		ctx->cb.mouse.device = device;
		ctx->cb.mouse.down = down;
		ctx->cb.mouse.move = move;
		ctx->cb.mouse.up = up;
		ctx->cb.mouse.wheel = wheel;
	}
}

void sandbox_event_set_touch_callback(void * context, void * device,
		void (*begin)(void * device, int x, int y, unsigned int id),
		void (*move)(void * device, int x, int y, unsigned int id),
		void (*end)(void * device, int x, int y, unsigned int id))
{
	struct sandbox_event_context_t * ctx = (struct sandbox_event_context_t *)context;
	if(ctx)
	{
		ctx->cb.touch.device = device;
		ctx->cb.touch.begin = begin;
		ctx->cb.touch.move = move;
		ctx->cb.touch.end = end;
	}
}

void sandbox_event_set_joystick_callback(void * context, void * device,
		void (*left_stick)(void * device, int x, int y),
		void (*right_stick)(void * device, int x, int y),
		void (*left_trigger)(void * device, int v),
		void (*right_trigger)(void * device, int v),
		void (*button_down)(void * device, unsigned int button),
		void (*button_up)(void * device, unsigned int button))
{
	struct sandbox_event_context_t * ctx = (struct sandbox_event_context_t *)context;
	if(ctx)
	{
		ctx->cb.joystick.device = device;
		ctx->cb.joystick.left_stick = left_stick;
		ctx->cb.joystick.right_stick = right_stick;
		ctx->cb.joystick.left_trigger = left_trigger;
		ctx->cb.joystick.right_trigger = right_trigger;
		ctx->cb.joystick.button_down = button_down;
		ctx->cb.joystick.button_up = button_up;
	}
}
