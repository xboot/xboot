#ifndef __AES128_H__
#define __AES128_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define AES128_BLOCK_SIZE	(16)

struct aes128_ctx_t {
	uint8_t xkey[176];
};

void aes128_set_key(struct aes128_ctx_t * ctx, uint8_t * key);
void aes128_ecb_encrypt(struct aes128_ctx_t * ctx, uint8_t * in, uint8_t * out, int blks);
void aes128_ecb_decrypt(struct aes128_ctx_t * ctx, uint8_t * in, uint8_t * out, int blks);
void aes128_cbc_encrypt(struct aes128_ctx_t * ctx, uint8_t * iv, uint8_t * in, uint8_t * out, int blks);
void aes128_cbc_decrypt(struct aes128_ctx_t * ctx, uint8_t * iv, uint8_t * in, uint8_t * out, int blks);
void aes128_ctr_encrypt(struct aes128_ctx_t * ctx, uint64_t offset, uint8_t * in, uint8_t * out, int bytes);
void aes128_ctr_decrypt(struct aes128_ctx_t * ctx, uint64_t offset, uint8_t * in, uint8_t * out, int bytes);

#ifdef __cplusplus
}
#endif

#endif /* __AES128_H__ */
