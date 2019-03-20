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

struct sandbox_event_context_t {
	struct sandbox_event_callback_t cb;
	struct pollfd * pfd;
	int npfd;
	char ** devices;
	pthread_t thread;
};

static int sandbox_event_open_device(struct sandbox_event_context_t * ctx, const char * dev)
{
	struct pollfd * new_pfd;
	struct input_id id;
	char ** new_devices;
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
	new_devices = realloc(ctx->devices, sizeof(ctx->devices[0]) * (ctx->npfd + 1));
	if(new_devices == NULL)
		return -1;
	ctx->devices = new_devices;

	printf(	"add device %d: %s\n", ctx->npfd, dev);
	printf(	"  bus:      %04x\n"
			"  vendor    %04x\n"
			"  product   %04x\n"
			"  version   %04x\n", id.bustype, id.vendor, id.product, id.version);
	printf(	"  name:     \"%s\"\n", name);
	printf(	"  location: \"%s\"\n"
			"  id:       \"%s\"\n", location, idstr);
	printf(	"  version:  %d.%d.%d\n", version >> 16, (version >> 8) & 0xff, version & 0xff);

	ctx->pfd[ctx->npfd].fd = fd;
	ctx->pfd[ctx->npfd].events = POLLIN;
	ctx->devices[ctx->npfd] = strdup(dev);
	ctx->npfd++;
	return 0;
}

static int sandbox_event_close_device(struct sandbox_event_context_t * ctx, const char * dev)
{
	int i;
	for(i = 1; i < ctx->npfd; i++)
	{
		if(strcmp(ctx->devices[i], dev) == 0)
		{
			int count = ctx->npfd - i - 1;
			free(ctx->devices[i]);
			memmove(ctx->devices + i, ctx->devices + i + 1, sizeof(ctx->devices[0]) * count);
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
	default:
		key = code;
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
							break;
						case EV_KEY:
							switch(e.value)
							{
							case 0:
					        	if(cb->key.up)
					        		cb->key.up(cb->key.device, keycode_map(e.code));
					        	break;
							case 1:
					        	if(cb->key.down)
					        		cb->key.down(cb->key.device, keycode_map(e.code));
					        	break;
							case 2:
					        	if(cb->key.down)
					        		cb->key.down(cb->key.device, keycode_map(e.code));
					        	break;
							default:
								break;
							}
							break;
						case EV_REL:
							break;
						case EV_ABS:
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
						// printf("[%8ld.%06ld] %04x %04x %08x\r\n", e.time.tv_sec, e.time.tv_usec, e.type, e.code, e.value);
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
