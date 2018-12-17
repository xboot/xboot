#ifndef __SHA1_H__
#define __SHA1_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SHA1_DIGEST_SIZE	(20)

struct sha1_ctx_t {
	uint64_t count;
	uint8_t buf[64];
	uint32_t state[5];
};

void sha1_init(struct sha1_ctx_t * ctx);
void sha1_update(struct sha1_ctx_t * ctx, const void * data, int len);
const uint8_t * sha1_final(struct sha1_ctx_t * ctx);
const uint8_t * sha1_hash(const void * data, int len, uint8_t * digest);

#ifdef __cplusplus
}
#endif

#endif /* __SHA1_H__ */
