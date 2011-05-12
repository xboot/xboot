#ifndef __CONFIGS_H__
#define __CONFIGS_H__

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

/*
 * malloc heap size
 */
#define		CONFIG_HEAP_SIZE		(SZ_32M)


#endif /* __CONFIGS_H__ */
