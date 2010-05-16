#ifndef __IO_H__
#define __IO_H__

#include <configs.h>
#include <default.h>

/*
 * read a byte value from address.
 */
static inline x_u8 __readb(x_sys addr)
{
	return( *((volatile x_u8 *)(addr)) );
}

/*
 * read a word value from address.
 */
static inline x_u16 __readw(x_sys addr)
{
	return( *((volatile x_u16 *)(addr)) );
}

/*
 * read a long value from address.
 */
static inline x_u32 __readl(x_sys addr)
{
	return( *((volatile x_u32 *)(addr)) );
}

/*
 * write a byte value to address.
 */
static inline void __writeb(x_sys addr, x_u8 value)
{
	*((volatile x_u8 *)(addr)) = value;
}

/*
 * write a word value to address.
 */
static inline void __writew(x_sys addr, x_u16 value)
{
	*((volatile x_u16 *)(addr)) = value;
}

/*
 * write a long value to address.
 */
static inline void __writel(x_sys addr, x_u32 value)
{
	*((volatile x_u32 *)(addr)) = value;
}


#define readb(a)		__readb(a)
#define readw(a)		__readw(a)
#define readl(a)		__readl(a)

#define writeb(a, v)	__writeb(a, v)
#define writew(a, v)	__writew(a, v)
#define writel(a, v)	__writel(a, v)

void readsb(const void * port, void * dst, x_s32 count);
void readsw(const void * port, void * dst, x_s32 count);
void readsl(const void * port, void * dst, x_s32 count);

#endif /* __IO_H__ */
