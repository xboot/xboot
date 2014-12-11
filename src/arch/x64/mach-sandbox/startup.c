#include <sandbox.h>

extern int xboot_main(int argc, char * argv[]);

int main(int argc, char * argv[])
{
	sandbox_init(argc, argv);
	xboot_main(argc, argv);
	sandbox_exit();

	return 0;
}
