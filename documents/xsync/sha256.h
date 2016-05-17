#ifndef __SHA256_H__
#define __SHA256_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>

#define SHA256_DIGEST_SIZE	(32)

struct sha256_ctx_t {
	uint64_t count;
	uint8_t buf[64];
	uint32_t state[8];
};

void sha256_init(struct sha256_ctx_t * ctx);
void sha256_update(struct sha256_ctx_t * ctx, const void * data, int len);
const uint8_t * sha256_final(struct sha256_ctx_t * ctx);
const uint8_t * sha256_hash(const void * data, int len, uint8_t * digest);

#ifdef __cplusplus
}
#endif

#endif /* __SHA256_H__ */
