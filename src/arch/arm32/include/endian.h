#ifndef __ARM32_ENDIAN_H__
#define __ARM32_ENDIAN_H__

#ifdef __cplusplus
extern "C" {
#endif

#define LITTLE_ENDIAN	(0x1234)
#define BIG_ENDIAN		(0x4321)

#if ( !defined(__LITTLE_ENDIAN) && !defined(__BIG_ENDIAN) )
#define __LITTLE_ENDIAN
#endif

#if defined(__LITTLE_ENDIAN)
#define BYTE_ORDER		LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN)
#define BYTE_ORDER		BIG_ENDIAN
#else
#error "Unknown byte order!"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_ENDIAN_H__ */
