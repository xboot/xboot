#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <SDL.h>
#include <sandbox.h>

static struct sandbox_t __sandbox;

static size_t read_file_to_memory(const char * filename, char ** buffer)
{
	size_t len;
	char * buf;
	int fd;

	if(!filename || !buffer)
		return 0;

	if(sandbox_file_isfile(filename) != 0)
		return 0;

	fd = sandbox_file_open(filename, "r");
	if(fd < 0)
		return 0;

	len = sandbox_file_length(fd);
	if(len <= 0)
		return 0;

	buf = malloc(len + 1);
	if(!buf)
		return 0;
	memset(buf, 0, len + 1);

	sandbox_file_seek(fd, 0);
	sandbox_file_read(fd, buf, len);
	sandbox_file_close(fd);

	*buffer = buf;
	return len;
}

static void print_usage(void)
{
	printf(
		"Usage: xboot [OPTIONS] <application>\n"
		"Options:\n"
		"  --help  Print help information\n"
		"  --json <FILE>  Start xboot with a specified file of device tree using json format\n"
	);
	exit(0);
}

struct sandbox_t * sandbox_get(void)
{
	return &__sandbox;
}

void sandbox_init(int argc, char * argv[])
{
	char path[PATH_MAX];
	int i, idx = 0;
	char * buf;
	size_t len;

	memset(&__sandbox, 0, sizeof(struct sandbox_t));

	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "--help"))
		{
			print_usage();
		}
		else if(!strcmp(argv[i], "--json") && (argc > i + 1))
		{
			if(sandbox_file_isfile(argv[++i]) == 0)
			{
				if((len = read_file_to_memory(argv[i], &buf)) > 0)
				{
					__sandbox.json.buffer = buf;
					__sandbox.json.size = len;
				}
			}
			else
			{
				print_usage();
			}
		}
		else
		{
			if(idx == 0)
			{
				if((sandbox_file_isdir(argv[i]) == 0) || (sandbox_file_isfile(argv[i]) == 0))
					__sandbox.app = strdup(realpath(argv[i], path));
			}
			else
				print_usage();
			idx++;
		}
	}

	SDL_Init(SDL_INIT_EVERYTHING);
	sandbox_stdio_init();
	sandbox_sdl_event_init();
}

void sandbox_exit(void)
{
	if(__sandbox.json.buffer && (__sandbox.json.size > 0))
		free(__sandbox.json.buffer);
	if(__sandbox.app)
		free(__sandbox.app);

	sandbox_sdl_timer_exit();
	sandbox_sdl_event_exit();
	sandbox_stdio_exit();
	SDL_Quit();
	exit(0);
}
