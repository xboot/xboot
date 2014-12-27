#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandbox.h>

static const char default_json_config[] =
"{"
	"\"console\": {"
		"\"in\" : \"stdio\","
		"\"out\": \"stdio\","
		"\"err\": \"stdio\","
	"},"
	""
	"\"framebuffer\": {"
		"\"width\": 640,"
		"\"height\": 480,"
		"\"xdpi\": 160,"
		"\"ydpi\": 160,"
		"\"fullscreen\": false"
	"},"
	""
	"\"input\": ["
		"\"keyboard\","
		"\"mouse\","
		"\"touchscreen\","
		"\"joystick\""
	"],"
"}";

static struct sandbox_config_t __sandbox_config = {
	.json			= default_json_config,
	.application	= NULL,
};

static char * json_config_file(const char * filename)
{
	ssize_t len;
	char * buf;
	int fd;

	fd = sandbox_file_open(filename);
	if(fd <= 0)
		return NULL;

	len = sandbox_file_length(fd);
	if(len <= 0)
		return NULL;

	buf = malloc(len + 1);
	if(!buf)
		return NULL;
	memset(buf, 0, len + 1);

	sandbox_file_seek(fd, 0);
	sandbox_file_read(fd, buf, len);
	return buf;
}

static void print_usage(void)
{
	printf(
		"Usage: xboot [OPTIONS] <filename>\n"
		"Options:\n"
		"  --help           Print help information\n"
		"  --config <json>  Start xboot with a specified config file with json format\n"
	);
	exit(0);
}

struct sandbox_config_t * sandbox_get_config(void)
{
	return &__sandbox_config;
}

void sandbox_init(int argc, char * argv[])
{
	char * jsonfile = "/etc/xboot.json";
	int i, idx = 0;
	char * json;

	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "--help"))
		{
			print_usage();
		}
		else if(!strcmp(argv[i], "--config") && (argc > i + 1))
		{
			jsonfile = argv[++i];
		}
		else
		{
			if(idx == 0)
			{
				__sandbox_config.application = argv[i];
			}
			else if(idx >= 1)
			{
				print_usage();
			}
			idx++;
		}
	}

	/*
	 * Read config file with json format
	 */
	json = json_config_file(jsonfile);
	if(json)
		__sandbox_config.json = json;

	/*
	 * Initial sandbox system
	 */
	sandbox_console_init();
	SDL_Init(SDL_INIT_EVERYTHING);
}

void sandbox_exit(void)
{
	if(__sandbox_config.json != default_json_config)
		free((char *)__sandbox_config.json);

	/*
	 * Deinitial sandbox system
	 */
	sandbox_console_exit();
	sandbox_sdl_timer_exit();
	SDL_Quit();
	exit(0);
}
