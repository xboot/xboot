#ifndef __EXPAT_CONFIG_H__
#define __EXPAT_CONFIG_H__

#include <xboot.h>

/* 1234 = LIL_ENDIAN, 4321 = BIGENDIAN */
#if defined(__LITTLE_ENDIAN)
# define BYTEORDER 1234
#elif defined(__BIG_ENDIAN)
# define BYTEORDER 4321
#else
# error "Unknown byte order!"
#endif

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to specify how much context to retain around the current parse point. */
#define XML_CONTEXT_BYTES 1024

/* Define to make parameter entity parsing functionality available. */
#define XML_DTD 1

/* Define to make XML Namespaces functionality available. */
#define XML_NS 1

#endif  /* __EXPAT_CONFIG_H__ */
