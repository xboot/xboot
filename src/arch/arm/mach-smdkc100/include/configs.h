#ifndef __CONFIGS_H__
#define __CONFIGS_H__

/*
 * define the basic data types.
 */
typedef signed char				x_s8;
typedef unsigned char			x_u8;

typedef signed short int		x_s16;
typedef unsigned short int		x_u16;

typedef signed long	int			x_s32;
typedef unsigned long int		x_u32;

typedef signed long long		x_s64;
typedef unsigned long long		x_u64;

typedef signed int				x_bool;
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
