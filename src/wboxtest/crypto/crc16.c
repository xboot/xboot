/*
 * wboxtest/crypto/crc16.c
 */

#include <crc16.h>
#include <wboxtest.h>

static void * crc16_setup(struct wboxtest_t * wbt)
{
	return NULL;
}

static void crc16_clean(struct wboxtest_t * wbt, void * data)
{
}

static void crc16_run(struct wboxtest_t * wbt, void * data)
{
	uint8_t msg[5] = { 'x', 'b', 'o', 'o', 't' };
	uint16_t crc = 0;

	crc = crc16_sum(crc, msg, sizeof(msg));
	assert_equal(crc, 0x394c);
}

static struct wboxtest_t wbt_crc16 = {
	.group	= "crypto",
	.name	= "crc16",
	.setup	= crc16_setup,
	.clean	= crc16_clean,
	.run	= crc16_run,
};

static __init void crc16_wbt_init(void)
{
	register_wboxtest(&wbt_crc16);
}

static __exit void crc16_wbt_exit(void)
{
	unregister_wboxtest(&wbt_crc16);
}

wboxtest_initcall(crc16_wbt_init);
wboxtest_exitcall(crc16_wbt_exit);
