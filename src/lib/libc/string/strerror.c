/*
 * libc/string/strerror.c
 */

#include <string.h>

char * strerror(int num)
{
	char * p;

	switch(num)
	{
	//TODO

	default:
		p = "Unknown error";
		break;
	}

	return p;
}
