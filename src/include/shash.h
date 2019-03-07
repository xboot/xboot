#ifndef __SHASH_H__
#define __SHASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

static inline __attribute__((always_inline)) uint32_t shash(const char * s)
{
	uint32_t v = 5381;
	if(s)
	{
		while(*s)
			v = (v << 5) + v + (*s++);
	}
	return v;
}

#ifdef __cplusplus
}
#endif

#endif /* __SHASH_H__ */
