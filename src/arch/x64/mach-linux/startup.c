#include <xboot.h>
#include <sandbox.h>

extern void xboot_main(void);

void do_autoboot(void)
{
	struct sandbox_t * sandbox = sandbox_get();
	int delay = CONFIG_AUTO_BOOT_DELAY * 1000;

	do {
		if(getchar() != EOF)
		{
			printf("\r\n");
			return;
		}
		mdelay(10);
		delay -= 10;
		if(delay < 0)
			delay = 0;
		printf("\rPress any key to stop autoboot:%3d.%03d%s", delay / 1000, delay % 1000, (delay == 0) ? "\r\n" : "");
	} while(delay > 0);

	if(sandbox && sandbox->app)
		shell_system(sandbox->app);
	else
		shell_system(CONFIG_AUTO_BOOT_COMMAND);
}

int main(int argc, char * argv[])
{
	sandbox_init(argc, argv);
	xboot_main();
	sandbox_exit();
	return 0;
}
