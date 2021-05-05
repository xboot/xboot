#ifndef __BASE64_H__
#define __BASE64_H__

#ifdef __cplusplus
extern "C" {
#endif

static inline int base64_encode_size(int s)
{
	return (int)(((s + 2) / 3) * 4 + 1);
}

static inline int base64_decode_size(int s)
{
	return (int)((s / 4) * 3);
}

int base64_encode(const char * in, int len, char * out);
int base64_decode(const char * in, int len, char * out);

#ifdef __cplusplus
}
#endif

#endif /* __BASE64_H__ */
