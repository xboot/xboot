#ifndef __SHA1_H__
#define __SHA1_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>
#include <string.h>

struct sha1_ctx {
	uint64_t count;
	uint8_t buf[64];
	uint32_t state[5];
};

void sha1_init(struct sha1_ctx * ctx);
void sha1_update(struct sha1_ctx * ctx, const void * data, int len);
const uint8_t * sha1_final(struct sha1_ctx * ctx);
const uint8_t * sha1_hash(const void * data, int len, uint8_t * digest);

#ifdef __cplusplus
}
#endif

#endif /* __SHA1_H__ */
