#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <termios.h>
#include <SDL.h>
#include <sandboxlinux.h>

static struct sandbox_config_t __sandbox_linux_config = {
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
static struct termios __term_config;

static void rawmode(void)
{
	struct termios termvi;

	tcgetattr(0, &__term_config);
	termvi = __term_config;
	termvi.c_lflag &= (~ICANON & ~ECHO & ~ISIG);
	termvi.c_iflag &= (~IXON & ~ICRNL);
	termvi.c_oflag |= (ONLCR);
	termvi.c_cc[VMIN] = 1;
	termvi.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &termvi);
}

static void cookmode(void)
{
	fflush(stdout);
	tcsetattr(0, TCSANOW, &__term_config);
}

struct sandbox_config_t * sandbox_linux_get_config(void)
{
	return &__sandbox_linux_config;
}

static void usage(const char * name)
{
	printf(
		"Usage: %s [OPTIONS] <filename>\n"
		"Options:\n"
		"  --help           Print help information\n"
		"  --malloc <size>  Start xboot with a specified memory malloc space size in MB\n"
		"  --width <size>   SDL framebuffer width\n"
		"  --height <size>  SDL framebuffer height\n"
		"  --xdpi <value>   SDL xdpi\n"
		"  --ydpi <value>   SDL ydpi\n"
		,name);
}

void sandbox_linux_init(int argc, char * argv[])
{
	int i, idx = 0;

	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "--help"))
		{
			usage(basename(argv[0]));
			exit(0);
			return;
		}
		else if(!strcmp(argv[i], "--malloc") && (argc > i + 1))
		{
			__sandbox_linux_config.memory.size = strtoul(argv[i + 1], NULL, 0) * 1024 * 1024;
			i++;
		}
		else if(!strcmp(argv[i], "--width") && (argc > i + 1))
		{
			__sandbox_linux_config.framebuffer.width = strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else if(!strcmp(argv[i], "--height") && (argc > i + 1))
		{
			__sandbox_linux_config.framebuffer.height = strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else if(!strcmp(argv[i], "--xdpi") && (argc > i + 1))
		{
			__sandbox_linux_config.framebuffer.xdpi = strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else if(!strcmp(argv[i], "--ydpi") && (argc > i + 1))
		{
			__sandbox_linux_config.framebuffer.ydpi = strtoul(argv[i + 1], NULL, 0);
			i++;
		}
		else
		{
			if(idx == 0)
			{
				__sandbox_linux_config.file.name = argv[i];
			}
			else if(idx >= 1)
			{
				usage(basename(argv[0]));
				exit(-1);
				return;
			}
			idx++;
		}
	}

	__sandbox_linux_config.memory.mem = malloc(__sandbox_linux_config.memory.size);
	if(!__sandbox_linux_config.memory.mem)
	{
		printf("Unable to malloc memory space\n");
		exit(1);
		return;
	}
	memset(__sandbox_linux_config.memory.mem, 0, __sandbox_linux_config.memory.size);

	rawmode();
	SDL_Init(SDL_INIT_EVERYTHING);
}

void sandbox_linux_exit(void)
{
	sandbox_linux_sdl_event_exit();
	sandbox_linux_sdl_timer_exit();
	sandbox_linux_sdl_fb_exit();
	cookmode();

	if(__sandbox_linux_config.memory.mem)
		free(__sandbox_linux_config.memory.mem);
	SDL_Quit();
	exit(0);
}
