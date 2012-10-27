#ifndef __ARM_ENDIAN_H__
#define __ARM_ENDIAN_H__

#define __LITTLE_ENDIAN

#if defined(__LITTLE_ENDIAN)
# undef  __LITTLE_ENDIAN
# define __LITTLE_ENDIAN	1234
# define __BIG_ENDIAN		4321
# define __PDP_ENDIAN		3412
# define __BYTE_ORDER		__LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN)
# undef  __BIG_ENDIAN
# define __LITTLE_ENDIAN	1234
# define __BIG_ENDIAN		4321
# define __PDP_ENDIAN		3412
# define __BYTE_ORDER		__BIG_ENDIAN
#elif defined(__PDP_ENDIAN)
# undef  __PDP_ENDIAN
# define __LITTLE_ENDIAN	1234
# define __BIG_ENDIAN		4321
# define __PDP_ENDIAN		3412
# define __BYTE_ORDER		__PDP_ENDIAN
#else
# error "Unknown byte order!"
#endif

#endif /* __ARM_ENDIAN_H__ */
