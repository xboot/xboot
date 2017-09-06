#include <sandbox.h>
#include <string.h>
#include <stdio.h>
#include <3ds.h>

static struct sandbox_t __sandbox;

struct sandbox_t * sandbox_get(void)
{
	return &__sandbox;
}

void sandbox_init(int argc, char * argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
	printf("XBOOT 3DS O3 test World!");

	while (aptMainLoop())
	{
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break;

		gfxFlushBuffers();
		gfxSwapBuffers();
	}
	gfxExit();
}

void sandbox_exit(void)
{
}
