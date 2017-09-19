#include <xboot.h>
#include <sandbox.h>

extern int xboot_main(int argc, char * argv[]);

const char * auto_boot_command(void)
{
	struct sandbox_t * sandbox = sandbox_get();
	if(sandbox && sandbox->app)
		return sandbox->app;
	return CONFIG_AUTO_BOOT_COMMAND;
}

int main(int argc, char * argv[])
{
	sandbox_init(argc, argv);
	xboot_main(argc, argv);
	sandbox_exit();
	return 0;
}
