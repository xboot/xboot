#ifndef __CAIRO_CONF_H__
#define __CAIRO_CONF_H__

#include <xboot.h>

#undef likely
#undef unlikely

#if defined(__BIG_ENDIAN)
# define WORDS_BIGENDIAN		(1)
#endif

#define CAIRO_NO_MUTEX			(1)
#define HAVE_UINT64_T			(1)
#define HAVE_STDINT_H			(1)

#endif /* __CAIRO_CONF_H__ */


