#ifndef __ARM32_ENDIAN_H__
#define __ARM32_ENDIAN_H__

#ifdef __cplusplus
extern "C" {
#endif

#if ( !defined(__LITTLE_ENDIAN) && !defined(__BIG_ENDIAN) )
# define __LITTLE_ENDIAN
#endif

#if defined(__LITTLE_ENDIAN)
# define LITTLE_ENDIAN	1234
# define BIG_ENDIAN		4321
# define BYTE_ORDER		LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN)
# define LITTLE_ENDIAN	1234
# define BIG_ENDIAN		4321
# define BYTE_ORDER		BIG_ENDIAN
#else
# error "Unknown byte order!"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_ENDIAN_H__ */
