#ifndef __BCD_H__
#define __BCD_H__

#ifdef __cplusplus
extern "C" {
#endif

static inline unsigned char bcd2bin(unsigned char x)
{
	return (((x) & 0x0f) + ((x) >> 4) * 10);
}

static inline unsigned char bin2bcd(unsigned char x)
{
	return ((((x) / 10) << 4) + (x) % 10);
}

#ifdef __cplusplus
}
#endif

#endif /* __BCD_H__ */
