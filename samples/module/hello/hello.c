/*
 * hello.c
 */

#include <xboot.h>
#include <hello.h>

static int test_value = 100;

int add(int i, int j)
{
	return (i+j);
}

void hello_main(void)
{
	int bb = 20;

	printk("abc\r\n");
	printk("add(2, 3)=%d\r\n", add(2, 3));
	printk("add(test_value, 3)=%d\r\n", add(test_value, 3));
	printk("bb=%d\r\n", bb);
}

