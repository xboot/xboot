#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandbox.h>

static const char __default_json[] =
"{"
	"\"input\": [\"keyboard\", \"mouse\", \"touchscreen\", \"joystick\"],"
	"\"memory\": {"
        "\"device_type\": [\"strings\", \"memory\"],"
        "\"reg\": [\"words\", \"0x00000000\", \"0x00000000\"],"
        "\"cfg\": [\"bytes\", \"12\", \"23\", \"-64\"]"
	"},"
	"\"chosen\" : {"
		"\"cmdline\": [\"strings\", \"debug=on\"]"
	"}"
"}";

static struct sandbox_config_t __sandbox_config = {
	.json				= (char *)__default_json,

	.file				= {
		.name			= NULL,
	},

	.memory				= {
		.mem			= NULL,
		.size			= 128 * 1024 * 1024,
	},

	.framebuffer		= {
		.width			= 640,
		.height			= 480,
		.xdpi			= 160,
		.ydpi			= 160,
	},
};

struct sandbox_config_t * sandbox_get_config(void)
{
	return &__sandbox_config;
}

static void usage(void)
{
	printf(
		"Usage: xboot [OPTIONS] <filename>\n"
		"Options:\n"
		"  --help           Print help information\n"
		"  --malloc <size>  Start xboot with a specified memory malloc space size in MB\n"
		"  --width <size>   SDL framebuffer width\n"
		"  --height <size>  SDL framebuffer height\n"
		"  --xdpi <value>   SDL xdpi\n"
		"  --ydpi <value>   SDL ydpi\n"
	);
}

void sandbox_init(int argc, char * argv[])
{
	int i, idx = 0;

	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "--help"))
		{
			usage();
			exit(0);
			return;
		}
		else if(!strcmp(argv[i], "--malloc") && (argc > i + 1))
		{
			__sandbox_config.memory.size = strtoul(argv[i + 1], NULL, 0) * 1024 * 1024;
			i++;
		}
		else if(!strcmp(argv[i], "--width") && (argc > i + 1))
		{
			__sandbox_config.framebuffer.width = strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else if(!strcmp(argv[i], "--height") && (argc > i + 1))
		{
			__sandbox_config.framebuffer.height = strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else if(!strcmp(argv[i], "--xdpi") && (argc > i + 1))
		{
			__sandbox_config.framebuffer.xdpi = strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else if(!strcmp(argv[i], "--ydpi") && (argc > i + 1))
		{
			__sandbox_config.framebuffer.ydpi = strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else
		{
			if(idx == 0)
			{
				__sandbox_config.file.name = argv[i];
			}
			else if(idx >= 1)
			{
				usage();
				exit(-1);
				return;
			}
			idx++;
		}
	}

	__sandbox_config.memory.mem = malloc(__sandbox_config.memory.size);
	if(!__sandbox_config.memory.mem)
	{
		printf("Unable to malloc memory space\n");
		exit(1);
		return;
	}
	memset(__sandbox_config.memory.mem, 0, __sandbox_config.memory.size);

	SDL_Init(SDL_INIT_EVERYTHING);
}

void sandbox_exit(void)
{
	sandbox_console_exit();
	sandbox_sdl_event_exit();
	sandbox_sdl_timer_exit();
	sandbox_sdl_fb_exit();

	if(__sandbox_config.memory.mem)
		free(__sandbox_config.memory.mem);
	SDL_Quit();
	exit(0);
}
