#include <x.h>
#include <sandbox.h>

const char * sandbox_uniqueid(void)
{
	return NULL;
}

int sandbox_keygen(const char * msg, void * key)
{
	return 0;
}

int sandbox_verify(void)
{
	return 1;
}
