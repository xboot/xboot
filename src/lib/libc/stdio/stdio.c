/*
 * libc/stdio/stdio.c
 */

#include <stdio.h>

static FILE __stdin;
FILE * stdin = &__stdin;

static FILE __stdout;
FILE * stdout = &__stdout;

static FILE __stderr;
FILE * stderr = &__stderr;
