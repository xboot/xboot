/*
 * module-hello.c
 */

#include <test.h>
#include <module-hello.h>

void module_hello_main(void)
{
	int i = 1, j = 2;

	i = test_add(i, j);
}
