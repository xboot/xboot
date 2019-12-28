/*
 * wboxtest/path/dirname.c
 */

#include <wboxtest.h>

static void * dirname_setup(struct wboxtest_t * wbt)
{
	return NULL;
}

static void dirname_clean(struct wboxtest_t * wbt, void * data)
{
}

static void dirname_run(struct wboxtest_t * wbt, void * data)
{
	char path[VFS_MAX_PATH];

	sprintf(path, "%s", "/usr/lib");
	assert_string_equal(dirname(path), "/usr");

	sprintf(path, "%s", "/usr/");
	assert_string_equal(dirname(path), "/");

	sprintf(path, "%s", "usr");
	assert_string_equal(dirname(path), ".");

	sprintf(path, "%s", "/");
	assert_string_equal(dirname(path), "/");

	sprintf(path, "%s", ".");
	assert_string_equal(dirname(path), ".");

	sprintf(path, "%s", "..");
	assert_string_equal(dirname(path), ".");
}

static struct wboxtest_t wbt_dirname = {
	.group	= "path",
	.name	= "dirname",
	.setup	= dirname_setup,
	.clean	= dirname_clean,
	.run	= dirname_run,
};

static __init void dirname_wbt_init(void)
{
	register_wboxtest(&wbt_dirname);
}

static __exit void dirname_wbt_exit(void)
{
	unregister_wboxtest(&wbt_dirname);
}

wboxtest_initcall(dirname_wbt_init);
wboxtest_exitcall(dirname_wbt_exit);
