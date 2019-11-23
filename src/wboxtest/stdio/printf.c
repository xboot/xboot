/*
 * wboxtest/stdio/printf.c
 */

#include <wboxtest.h>

static void * printf_setup(struct wboxtest_t * wbt)
{
	return NULL;
}

static void printf_clean(struct wboxtest_t * wbt, void * data)
{
}

static void printf_run(struct wboxtest_t * wbt, void * data)
{
	printf(" hello world!\r\n");
}

static struct wboxtest_t wbt_printf = {
	.group	= "stdio",
	.name	= "printf",
	.setup	= printf_setup,
	.clean	= printf_clean,
	.run	= printf_run,
};

static __init void printf_wbt_init(void)
{
	register_wboxtest(&wbt_printf);
}

static __exit void printf_wbt_exit(void)
{
	unregister_wboxtest(&wbt_printf);
}

wboxtest_initcall(printf_wbt_init);
wboxtest_exitcall(printf_wbt_exit);
