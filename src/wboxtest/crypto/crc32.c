/*
 * wboxtest/crypto/crc32.c
 */

#include <crc32.h>
#include <wboxtest.h>

static void * crc32_setup(struct wboxtest_t * wbt)
{
	return NULL;
}

static void crc32_clean(struct wboxtest_t * wbt, void * data)
{
}

static void crc32_run(struct wboxtest_t * wbt, void * data)
{
	uint8_t msg[5] = { 'x', 'b', 'o', 'o', 't' };
	uint32_t crc = 0;

	crc = crc32_sum(crc, msg, sizeof(msg));
	assert_equal(crc, 0x68292dcb);
}

static struct wboxtest_t wbt_crc32 = {
	.group	= "crypto",
	.name	= "crc32",
	.setup	= crc32_setup,
	.clean	= crc32_clean,
	.run	= crc32_run,
};

static __init void crc32_wbt_init(void)
{
	register_wboxtest(&wbt_crc32);
}

static __exit void crc32_wbt_exit(void)
{
	unregister_wboxtest(&wbt_crc32);
}

wboxtest_initcall(crc32_wbt_init);
wboxtest_exitcall(crc32_wbt_exit);
