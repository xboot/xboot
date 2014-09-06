#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <termios.h>
#include <SDL.h>
#include <sandboxlinux.h>

static struct sandbox_config_t __sandbox_linux_config = {
	.framebuffer	= {
		.width		= 640,
		.height		= 480,
		.xdpi		= 160,
		.ydpi		= 160,
		.bpp		= 32,
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

void sandbox_linux_init(int argc, char * argv[])
{
	rawmode();
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE);
}

void sandbox_linux_exit(void)
{
	cookmode();
	sandbox_linux_sdl_event_exit();
	sandbox_linux_sdl_timer_exit();
	sandbox_linux_sdl_fb_exit();
	SDL_Quit();
	exit(0);
}
