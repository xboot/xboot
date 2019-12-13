#include <sandbox.h>

extern void xboot_main(void);

int main(int argc, char * argv[])
{
	sandbox_init(argc, argv);
//	xboot_main();
	sandbox_exit();
	return 0;
}
