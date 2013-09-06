#ifndef __PIXMAN_CONF_H__
#define __PIXMAN_CONF_H__

#include <xboot.h>

#if defined(__BIG_ENDIAN)
# define WORDS_BIGENDIAN		(1)
#endif

#define PIXMAN_NO_TLS 			(1)

#if	ULONG_MAX == 0xFFFFFFFFUL
#define SIZEOF_LONG 			(32 / CHAR_BIT)
#elif ULONG_MAX == 0xFFFFFFFFFFFFFFFFUL
#define SIZEOF_LONG 			(64 / CHAR_BIT)
#else
#error "Unsupported size of 'long' type!"
#endif

#endif /* __PIXMAN_CONF_H__ */


