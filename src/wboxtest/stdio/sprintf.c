/*
 * wboxtest/stdio/sprintf.c
 */

#include <wboxtest.h>

static void * sprintf_setup(struct wboxtest_t * wbt)
{
	return NULL;
}

static void sprintf_clean(struct wboxtest_t * wbt, void * data)
{
}

static void sprintf_run(struct wboxtest_t * wbt, void * data)
{
	char buf[SZ_1K];

	sprintf(buf, "%d = %i^%s", 1024, 2, "10");
	assert_string_equal(buf, "1024 = 2^10");

	sprintf(buf, "0x%x = 0X%X", 0xbad, 2989);
	assert_string_equal(buf, "0xbad = 0XBAD");

	sprintf(buf, "%d + %d + %d = %d", 1, 1, 778, 780);
	assert_string_equal(buf, "1 + 1 + 778 = 780");

	sprintf(buf, "%d %% %u = %i", 42, 2, 0);
	assert_string_equal(buf, "42 % 2 = 0");

	sprintf(buf, "%c%c%c%c%c", 'x', 'b', 'o', 'o', 't');
	assert_string_equal(buf, "xboot");
}

static struct wboxtest_t wbt_sprintf = {
	.group	= "stdio",
	.name	= "sprintf",
	.setup	= sprintf_setup,
	.clean	= sprintf_clean,
	.run	= sprintf_run,
};

static __init void sprintf_wbt_init(void)
{
	register_wboxtest(&wbt_sprintf);
}

static __exit void sprintf_wbt_exit(void)
{
	unregister_wboxtest(&wbt_sprintf);
}

wboxtest_initcall(sprintf_wbt_init);
wboxtest_exitcall(sprintf_wbt_exit);
