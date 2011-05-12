#ifndef __CONFIGS_H__
#define __CONFIGS_H__

/*
 * define the basic data types.
 */
typedef signed char				s8_t;
typedef unsigned char			u8_t;

typedef signed short int		s16_t;
typedef unsigned short int		u16_t;

typedef long signed int			s32_t;
typedef long unsigned int		u32_t;

typedef signed long long		s64_t;
typedef unsigned long long		u64_t;

typedef signed int				bool_t;
typedef unsigned int			x_sys;
typedef signed long long		x_size;
typedef signed long long		x_off;
typedef void*					x_addr;

/*
 * define the format of endian, little endian or big endian.
 */
#define		__LITTLE_ENDIAN
#undef		__BIG_ENDIAN

/*
 * define the bits of system, 32bits or 64bits.
 */
#define		__SYS_32BIT
#undef		__SYS_64BIT


#endif /* __CONFIGS_H__ */
