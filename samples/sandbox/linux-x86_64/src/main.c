#include <sandbox/sandbox.h>

extern int xboot_main(int argc, char * argv[]);
struct sandbox_t * sandbox;

int main(int argc, char * argv[])
{
	sandbox = sandbox_alloc();

	xboot_main(argc, argv);

	sandbox_free(sandbox);
	return 0;
}
