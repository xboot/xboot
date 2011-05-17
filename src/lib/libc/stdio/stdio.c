/*
 * libc/stdio/stdio.c
 */

#include <stdio.h>


static FILE __stdin = {
	.fd		= 0,
};
FILE * stdin = &__stdin;


static FILE __stdout = {
	.fd		= 1,
};
FILE * stdout = &__stdout;


static FILE __stderr = {
	.fd		= 2,
};
FILE * stderr = &__stderr;
