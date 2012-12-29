#ifndef __RSA_H__
#define __RSA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>

/*
 * 2048 bit key length
 */
struct rsa_public_key {
	u32_t len;						/* Length of n[] in number of u32_t */
    u32_t n0inv;					/* -1/n[0] mod 2^32 */
    u32_t n[256/sizeof(u32_t)];		/* modulus as little endian array */
    u32_t rr[256/sizeof(u32_t)];	/* R^2 as little endian array */
};

bool_t rsa_verify(const struct rsa_public_key * key, const u8_t * signature, const u8_t * sha);

#ifdef __cplusplus
}
#endif

#endif /* __RSA_H__ */
