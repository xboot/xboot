#ifndef __IO_H__
#define __IO_H__

#include <configs.h>
#include <default.h>

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

#define writeb(a, v)	__writeb(a, v)
#define writew(a, v)	__writew(a, v)
#define writel(a, v)	__writel(a, v)

#define readb(a)		__readb(a)
#define readw(a)		__readw(a)
#define readl(a)		__readl(a)


#define outb(a, v)		__writeb(a, v)
#define outw(a, v)		__writew(a, v)
#define outl(a, v)		__writel(a, v)

#define inb(a)			__readb(a)
#define inw(a)			__readw(a)
#define inl(a)			__readl(a)


#endif /* __IO_H__ */
