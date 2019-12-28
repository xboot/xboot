/*
 * wboxtest/path/basename.c
 */

#include <wboxtest.h>

static void * basename_setup(struct wboxtest_t * wbt)
{
	return NULL;
}

static void basename_clean(struct wboxtest_t * wbt, void * data)
{
}

static void basename_run(struct wboxtest_t * wbt, void * data)
{
	char path[VFS_MAX_PATH];

	sprintf(path, "%s", "/usr/lib");
	assert_string_equal(basename(path), "lib");

	sprintf(path, "%s", "/usr/");
	assert_string_equal(basename(path), "usr");

	sprintf(path, "%s", "usr");
	assert_string_equal(basename(path), "usr");

	sprintf(path, "%s", "/");
	assert_string_equal(basename(path), "/");

	sprintf(path, "%s", ".");
	assert_string_equal(basename(path), ".");

	sprintf(path, "%s", "..");
	assert_string_equal(basename(path), "..");
}

static struct wboxtest_t wbt_basename = {
	.group	= "path",
	.name	= "basename",
	.setup	= basename_setup,
	.clean	= basename_clean,
	.run	= basename_run,
};

static __init void basename_wbt_init(void)
{
	register_wboxtest(&wbt_basename);
}

static __exit void basename_wbt_exit(void)
{
	unregister_wboxtest(&wbt_basename);
}

wboxtest_initcall(basename_wbt_init);
wboxtest_exitcall(basename_wbt_exit);
