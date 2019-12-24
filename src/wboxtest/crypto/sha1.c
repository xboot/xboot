/*
 * wboxtest/crypto/sha1.c
 */

#include <sha1.h>
#include <wboxtest.h>

static void * sha1_setup(struct wboxtest_t * wbt)
{
	return NULL;
}

static void sha1_clean(struct wboxtest_t * wbt, void * data)
{
}

static void sha1_run(struct wboxtest_t * wbt, void * data)
{
	uint8_t msg[5] = { 'x', 'b', 'o', 'o', 't' };
	uint8_t digest[SHA1_DIGEST_SIZE] = {
		0x71, 0x82, 0xdf, 0x07, 0xc7, 0xf2, 0x06, 0x75,
		0x0f, 0xe2, 0xed, 0x98, 0x58, 0x0a, 0xb5, 0x26,
		0xcb, 0x83, 0x9f, 0x5d,
	};
	uint8_t result[SHA1_DIGEST_SIZE];

	sha1_hash(msg, sizeof(msg), result);
	assert_memory_equal(result, digest, SHA1_DIGEST_SIZE);
}

static struct wboxtest_t wbt_sha1 = {
	.group	= "crypto",
	.name	= "sha1",
	.setup	= sha1_setup,
	.clean	= sha1_clean,
	.run	= sha1_run,
};

static __init void sha1_wbt_init(void)
{
	register_wboxtest(&wbt_sha1);
}

static __exit void sha1_wbt_exit(void)
{
	unregister_wboxtest(&wbt_sha1);
}

wboxtest_initcall(sha1_wbt_init);
wboxtest_exitcall(sha1_wbt_exit);
