#ifndef __AES256_H__
#define __AES256_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define AES256_KEY_SIZE		(32)
#define AES256_BLOCK_SIZE	(16)

struct aes256_ctx_t {
	uint8_t xkey[4 * 8 * (14 + 1)];
};

void aes256_set_key(struct aes256_ctx_t * ctx, uint8_t * key);
void aes256_ecb_encrypt(struct aes256_ctx_t * ctx, uint8_t * in, uint8_t * out, int blks);
void aes256_ecb_decrypt(struct aes256_ctx_t * ctx, uint8_t * in, uint8_t * out, int blks);
void aes256_cbc_encrypt(struct aes256_ctx_t * ctx, uint8_t * iv, uint8_t * in, uint8_t * out, int blks);
void aes256_cbc_decrypt(struct aes256_ctx_t * ctx, uint8_t * iv, uint8_t * in, uint8_t * out, int blks);
void aes256_ctr_encrypt(struct aes256_ctx_t * ctx, uint64_t offset, uint8_t * in, uint8_t * out, int bytes);
void aes256_ctr_decrypt(struct aes256_ctx_t * ctx, uint64_t offset, uint8_t * in, uint8_t * out, int bytes);

#ifdef __cplusplus
}
#endif

#endif /* __AES256_H__ */
