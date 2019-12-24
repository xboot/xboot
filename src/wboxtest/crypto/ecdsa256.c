/*
 * wboxtest/crypto/ecdsa256.c
 */

#include <ecdsa256.h>
#include <wboxtest.h>

static void * ecdsa256_setup(struct wboxtest_t * wbt)
{
	return NULL;
}

static void ecdsa256_clean(struct wboxtest_t * wbt, void * data)
{
}

static void ecdsa256_run(struct wboxtest_t * wbt, void * data)
{
	uint8_t pub[ECDSA256_PUBLIC_KEY_SIZE];
	uint8_t priv[ECDSA256_PRIVATE_KEY_SIZE];
	uint8_t sign[ECDSA256_SIGNATURE_SIZE];
	uint8_t msg[32];

	wboxtest_random_buffer((char *)msg, sizeof(msg));
	assert_true(ecdsa256_keygen(pub, priv));
	assert_true(ecdsa256_sign(priv, msg, sign));
	assert_true(ecdsa256_verify(pub, msg, sign));
}

static struct wboxtest_t wbt_ecdsa256 = {
	.group	= "crypto",
	.name	= "ecdsa256",
	.setup	= ecdsa256_setup,
	.clean	= ecdsa256_clean,
	.run	= ecdsa256_run,
};

static __init void ecdsa256_wbt_init(void)
{
	register_wboxtest(&wbt_ecdsa256);
}

static __exit void ecdsa256_wbt_exit(void)
{
	unregister_wboxtest(&wbt_ecdsa256);
}

wboxtest_initcall(ecdsa256_wbt_init);
wboxtest_exitcall(ecdsa256_wbt_exit);
