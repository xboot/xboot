/*
 * wboxtest/crypto/base64.c
 */

#include <base64.h>
#include <wboxtest.h>

static void * base64_setup(struct wboxtest_t * wbt)
{
	return NULL;
}

static void base64_clean(struct wboxtest_t * wbt, void * data)
{
}

static void base64_run(struct wboxtest_t * wbt, void * data)
{
	char in[128];
	char out1[base64_encode_size(sizeof(in))];
	char out2[base64_decode_size(sizeof(out1))];

	wboxtest_random_buffer(in, sizeof(in));
	base64_encode(in, sizeof(in), out1);
	base64_decode(out1, sizeof(out1), out2);
	assert_memory_equal(in, out2, sizeof(in));
}

static struct wboxtest_t wbt_base64 = {
	.group	= "crypto",
	.name	= "base64",
	.setup	= base64_setup,
	.clean	= base64_clean,
	.run	= base64_run,
};

static __init void base64_wbt_init(void)
{
	register_wboxtest(&wbt_base64);
}

static __exit void base64_wbt_exit(void)
{
	unregister_wboxtest(&wbt_base64);
}

wboxtest_initcall(base64_wbt_init);
wboxtest_exitcall(base64_wbt_exit);
