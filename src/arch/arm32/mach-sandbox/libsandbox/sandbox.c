#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandbox.h>

static char default_config[] =
"{"
	"\"console\": {"
		"\"in\" : \"stdio\","
		"\"out\": \"stdio\","
		"\"err\": \"stdio\""
	"},"
	"\"framebuffer\": {"
		"\"width\": 640,"
		"\"height\": 480,"
		"\"xdpi\": 160,"
		"\"ydpi\": 160,"
		"\"fullscreen\": false"
	"},"
	"\"input\": ["
		"\"keyboard\","
		"\"mouse\","
		"\"touchscreen\","
		"\"joystick\""
	"],"
	"\"led\": [null]"
"}";

static struct sandbox_t __sandbox;

static size_t read_file_to_memory(const char * filename, char ** buffer)
{
	size_t len;
	char * buf;
	int fd;

	if(!filename || !buffer)
		return 0;

	if(sandbox_file_exist(filename) != 0)
		return 0;

	fd = sandbox_file_open(filename);
	if(fd <= 0)
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
		"  --help           Print help information\n"
		"  --config <FILE>  Start xboot with a specified config file using json format\n"
	);
	exit(0);
}

struct sandbox_t * sandbox_get(void)
{
	return &__sandbox;
}

void sandbox_init(int argc, char * argv[])
{
	char * cfgfile = "xboot.json";
	char * appfile = 0;
	int i, idx = 0;
	char * buf;
	size_t len;

	/*
	 * Clear to zero for __sandbox
	 */
	memset(&__sandbox, 0, sizeof(struct sandbox_t));

	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "--help"))
		{
			print_usage();
		}
		else if(!strcmp(argv[i], "--config") && (argc > i + 1))
		{
			if(sandbox_file_exist(argv[++i]) == 0)
				cfgfile = argv[i];
			else
				print_usage();
		}
		else
		{
			if((idx == 0) && (sandbox_file_exist(argv[i]) == 0))
				appfile = argv[i];
			else
				print_usage();
			idx++;
		}
	}

	/*
	 * Read config file
	 */
	len = read_file_to_memory(cfgfile, &buf);
	if(len > 0)
	{
		__sandbox.config.buffer = buf;
		__sandbox.config.size = len;
	}
	else
	{
		__sandbox.config.buffer = default_config;
		__sandbox.config.size = strlen(default_config);
	}

	/*
	 * Read application file
	 */
	len = read_file_to_memory(appfile, &buf);
	if(len > 0)
	{
		__sandbox.application.buffer = buf;
		__sandbox.application.size = len;
	}

	/*
	 * Initial sandbox system
	 */
	sandbox_console_init();
	SDL_Init(SDL_INIT_EVERYTHING);
}

void sandbox_exit(void)
{
	if((__sandbox.config.size > 0) && (__sandbox.config.buffer != default_config))
		free(__sandbox.config.buffer);

	if(__sandbox.application.size > 0)
		free(__sandbox.application.buffer);

	/*
	 * Deinitial sandbox system
	 */
	sandbox_console_exit();
	sandbox_sdl_timer_exit();
	SDL_Quit();
	exit(0);
}
