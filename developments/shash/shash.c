#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

static inline uint32_t shash(const char * s)
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
	if((argc != 2) || (strlen(argv[1]) == 0))
	{
		printf("Usage: shash <string>\n");
		return -1;
	}
	printf("0x%08x\r\n", shash(argv[1]));

	return 0;
}
