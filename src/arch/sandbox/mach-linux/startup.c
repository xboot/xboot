#include <sandbox-linux.h>

extern int xboot_main(int argc, char * argv[]);

int main(int argc, char * argv[])
{
	sandbox_linux_init();
	xboot_main(0, 0);
	sandbox_linux_exit();

	return 0;
}
