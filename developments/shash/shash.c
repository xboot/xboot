#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

static uint32_t shash(const char * s)
{
	uint32_t v = 5381;
	if(s)
	{
		while(*s)
			v = (v << 5) + v + (*s++);
	}
	return v;
}

int main (int argc, char * argv[])
{
	int i;

	if(argc <= 1)
	{
		printf("Usage: shash <string> ...\n");
		return -1;
	}
	for(i = 1; i < argc; i++)
	{
		if(argv[i] && strlen(argv[i]) > 0)
			printf("0x%08x: /* \"%s\" */\r\n", shash(argv[i]), argv[i]);
	}
	return 0;
}
