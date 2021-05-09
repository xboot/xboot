#ifndef __ECDSA256_H__
#define __ECDSA256_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ECDSA256_BYTES				(32)
#define ECDSA256_SHARED_KEY_SIZE	(ECDSA256_BYTES)
#define ECDSA256_PRIVATE_KEY_SIZE	(ECDSA256_BYTES)
#define ECDSA256_PUBLIC_KEY_SIZE	(ECDSA256_BYTES + 1)
#define ECDSA256_SIGNATURE_SIZE		(ECDSA256_BYTES * 2)

int ecdh256_keygen(const uint8_t * public, const uint8_t * private, uint8_t * shared);
int ecdsa256_keygen(uint8_t * public, uint8_t * private);
int ecdsa256_sign(const uint8_t * private, const uint8_t * sha256, uint8_t * signature);
int ecdsa256_verify(const uint8_t * public, const uint8_t * sha256, const uint8_t * signature);

#ifdef __cplusplus
}
#endif

#endif /* __ECDSA256_H__ */
