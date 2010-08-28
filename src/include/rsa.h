#ifndef __RSA_H__
#define __RSA_H__

#include <configs.h>
#include <default.h>
#include <types.h>

/*
 * 2048 bit key length
 */
struct rsa_public_key {
	x_u32 len;						/* Length of n[] in number of x_u32 */
    x_u32 n0inv;					/* -1/n[0] mod 2^32 */
    x_u32 n[256/sizeof(x_u32)];		/* modulus as little endian array */
    x_u32 rr[256/sizeof(x_u32)];	/* R^2 as little endian array */
};

x_bool rsa_verify(const struct rsa_public_key * key, const x_u8 * signature, const x_u8 * sha);

#endif /* __RSA_H__ */
