/*
 * wboxtest/crypto/crc8.c
 */

#include <crc8.h>
#include <wboxtest.h>

static void * crc8_setup(struct wboxtest_t * wbt)
{
	return NULL;
}

static void crc8_clean(struct wboxtest_t * wbt, void * data)
{
}

static void crc8_run(struct wboxtest_t * wbt, void * data)
{
	uint8_t msg[5] = { 'x', 'b', 'o', 'o', 't' };
	uint8_t crc = 0;

	crc = crc8_sum(crc, msg, sizeof(msg));
	assert_equal(crc, 0x01);
}

static struct wboxtest_t wbt_crc8 = {
	.group	= "crypto",
	.name	= "crc8",
	.setup	= crc8_setup,
	.clean	= crc8_clean,
	.run	= crc8_run,
};

static __init void crc8_wbt_init(void)
{
	register_wboxtest(&wbt_crc8);
}

static __exit void crc8_wbt_exit(void)
{
	unregister_wboxtest(&wbt_crc8);
}

wboxtest_initcall(crc8_wbt_init);
wboxtest_exitcall(crc8_wbt_exit);
