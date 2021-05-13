/*
 * libc/crypto/rc4.c
 */

#include <rc4.h>

void rc4_crypt(uint8_t * key, int kl, uint8_t * dat, int dl)
{
	uint8_t s[256], k[256];
	uint8_t t;
	int i, j, o;

	for(i = 0; i < 256; i++)
	{
		s[i] = (uint8_t)i;
		k[i] = key[i % kl];
	}
	for(i = 0, j = 0; i < 256; i++)
	{
		j = (j + s[i] + k[i]) & 0xff;
		t = s[i];
		s[i] = s[j];
		s[j] = t;
	}
	for(i = 0, j = 0, o = 0; o < dl; o++)
	{
		i = (i + 1) & 0xff;
		j = (j + s[i]) & 0xff;
		t = s[i];
		s[i] = s[j];
		s[j] = t;
		dat[o] ^= s[(s[i] + s[j]) & 0xff];
	}
}
