#ifndef __TYPES_H__
#define __TYPES_H__

#include <configs.h>
#include <default.h>

/**
 * define the macro of NULL.
 */
#if defined(__cplusplus)
#define NULL 			(0)
#else
#define NULL 			((void *)0)
#endif

/**
 * define the type of true and false.
 */
enum {
	FALSE	= 0,
	TRUE	= 1
};

/**
 * Add offset to address.
 * @param addr		original address
 * @param off		offset to add
 * @return new address
 */
extern inline x_addr addr_offset(x_addr addr, x_sys off)
{
    return (x_addr)((x_sys)addr + off);
}

/**
 * Apply mask to an address.
 * @param addr		original address
 * @param mask		address mask
 * @return new address
 */
extern inline x_addr addr_mask (x_addr addr, x_sys mask)
{
    return (x_addr)((x_sys) addr & mask);
}

/**
 * Align address downwards.  It is assumed that the alignment is a power of 2.
 * @param addr		original address
 * @param align		alignment
 * @return new address
 */
extern inline x_addr addr_align (x_addr addr, x_sys align)
{
    return addr_mask (addr, ~(align - 1));
}

/**
 * Align address upwards.  It is assumed that the alignment is a power of 2.
 * @param addr		original address
 * @param align		alignment
 * @return new address
 */
extern inline x_addr addr_align_up (x_addr addr, x_sys align)
{
    return addr_mask (addr_offset (addr, align - 1), ~(align - 1));
}

#define MAX(a, b) 		((a) > (b) ? (a) : (b))
#define MIN(a, b) 		((a) < (b) ? (a) : (b))
#define ABS(v) 			(((v)>0) ? (v) : -(v))


#endif /* __TYPES_H__ */
