/*
 * wboxtest/crypto/aes256.c
 */

#include <aes256.h>
#include <wboxtest.h>

static void * aes256_setup(struct wboxtest_t * wbt)
{
	return NULL;
}

static void aes256_clean(struct wboxtest_t * wbt, void * data)
{
}

static void aes256_run(struct wboxtest_t * wbt, void * data)
{
	struct aes256_ctx_t ctx;
	uint8_t in[AES256_BLOCK_SIZE * 10];
	uint8_t out1[AES256_BLOCK_SIZE * 10];
	uint8_t out2[AES256_BLOCK_SIZE * 10];
	uint8_t iv[AES256_BLOCK_SIZE];
	uint8_t key[AES256_KEY_SIZE];
	uint64_t offset;
	int bytes;

	wboxtest_random_buffer((char *)in, sizeof(in));
	wboxtest_random_buffer((char *)key, sizeof(key));
	aes256_set_key(&ctx, key);
	aes256_ecb_encrypt(&ctx, in, out1, 10);
	aes256_ecb_decrypt(&ctx, out1, out2, 10);
	assert_memory_equal(in, out2, sizeof(in));

	wboxtest_random_buffer((char *)in, sizeof(in));
	wboxtest_random_buffer((char *)iv, sizeof(iv));
	wboxtest_random_buffer((char *)key, sizeof(key));
	aes256_set_key(&ctx, key);
	aes256_cbc_encrypt(&ctx, iv, in, out1, 10);
	aes256_cbc_decrypt(&ctx, iv, out1, out2, 10);
	assert_memory_equal(in, out2, sizeof(in));

	offset = wboxtest_random_int(0, 10);
	bytes = wboxtest_random_int(0, sizeof(in));
	wboxtest_random_buffer((char *)in, sizeof(in));
	wboxtest_random_buffer((char *)key, sizeof(key));
	aes256_set_key(&ctx, key);
	aes256_ctr_encrypt(&ctx, offset, in, out1, bytes);
	aes256_ctr_decrypt(&ctx, offset, out1, out2, bytes);
	assert_memory_equal(in, out2, bytes);
}

static struct wboxtest_t wbt_aes256 = {
	.group	= "crypto",
	.name	= "aes256",
	.setup	= aes256_setup,
	.clean	= aes256_clean,
	.run	= aes256_run,
};

static __init void aes256_wbt_init(void)
{
	register_wboxtest(&wbt_aes256);
}

static __exit void aes256_wbt_exit(void)
{
	unregister_wboxtest(&wbt_aes256);
}

wboxtest_initcall(aes256_wbt_init);
wboxtest_exitcall(aes256_wbt_exit);
