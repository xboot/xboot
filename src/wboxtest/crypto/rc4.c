/*
 * wboxtest/crypto/rc4.c
 */

#include <rc4.h>
#include <wboxtest.h>

static void * rc4_setup(struct wboxtest_t * wbt)
{
	return NULL;
}

static void rc4_clean(struct wboxtest_t * wbt, void * data)
{
}

static void rc4_run(struct wboxtest_t * wbt, void * data)
{
	uint8_t key[5] = { 'x', 'b', 'o', 'o', 't' };
	uint8_t dat[256];
	uint8_t tmp[256];

	wboxtest_random_buffer((char *)dat, sizeof(dat));
	memcpy(tmp, dat, sizeof(dat));
	rc4_crypt(key, 5, tmp, sizeof(tmp));
	rc4_crypt(key, 5, tmp, sizeof(tmp));
	assert_memory_equal(dat, tmp, sizeof(dat));
}

static struct wboxtest_t wbt_rc4 = {
	.group	= "crypto",
	.name	= "rc4",
	.setup	= rc4_setup,
	.clean	= rc4_clean,
	.run	= rc4_run,
};

static __init void rc4_wbt_init(void)
{
	register_wboxtest(&wbt_rc4);
}

static __exit void rc4_wbt_exit(void)
{
	unregister_wboxtest(&wbt_rc4);
}

wboxtest_initcall(rc4_wbt_init);
wboxtest_exitcall(rc4_wbt_exit);
