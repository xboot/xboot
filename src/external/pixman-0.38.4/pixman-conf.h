#ifndef __PIXMAN_CONF_H__
#define __PIXMAN_CONF_H__

#include <xboot.h>

#undef likely
#undef unlikely

#if defined(__BIG_ENDIAN)
# define WORDS_BIGENDIAN		(1)
#endif

#if ULONG_MAX == 0xFFFFFFFFUL
#define SIZEOF_LONG 			(32 / CHAR_BIT)
#elif ULONG_MAX == 0xFFFFFFFFFFFFFFFFUL
#define SIZEOF_LONG 			(64 / CHAR_BIT)
#else
#error "Unsupported size of 'long' type!"
#endif

#ifdef __ARM32__
#if __ARM32_ARCH__ >= 6
# define USE_ARM_SIMD
#endif
#ifdef __ARM32_NEON__
# define USE_ARM_NEON
#endif
#endif

#ifdef __ARM64__
#ifdef __ARM64_NEON__
# define USE_ARM_A64_NEON
#endif
#endif

#ifdef __X64__
# define USE_X86_MMX
# define USE_SSE2
# define USE_SSSE3
#endif

#define PIXMAN_NO_TLS 			(1)
#define HAVE_BUILTIN_CLZ		(1)

#endif /* __PIXMAN_CONF_H__ */


