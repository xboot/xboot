#ifndef __AES128_H__
#define __AES128_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>
#include <string.h>

struct aes128_ctx {
	uint8_t xkey[176];
};

void aes128_set_key(struct aes128_ctx * ctx, uint8_t * key);
void aes128_encrypt(struct aes128_ctx * ctx, uint8_t * in, uint8_t * out);
void aes128_decrypt(struct aes128_ctx * ctx, uint8_t * in, uint8_t * out);

#ifdef __cplusplus
}
#endif

#endif /* __AES128_H__ */
