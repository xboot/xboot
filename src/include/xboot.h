#ifndef __XBOOT_H__
#define __XBOOT_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <debug.h>
#include <stdarg.h>
#include <macros.h>
#include <malloc.h>
#include <version.h>
#include <xboot/initcall.h>
#include <xboot/list.h>
#include <xboot/major.h>
#include <xboot/printk.h>
#include <xboot/panic.h>

/*
 * check the define of __LITTLE_ENDIAN and __BIG_ENDIAN
 */
#if	( defined(__LITTLE_ENDIAN) && defined(__BIG_ENDIAN) )
#error "please don't define __LITTLE_ENDIAN and __BIG_ENDIAN at the same time";
#elif ( !defined(__LITTLE_ENDIAN) && !defined(__BIG_ENDIAN) )
#error "you must define __LITTLE_ENDIAN or __BIG_ENDIAN before continue";
#endif

/*
 * check the define of __SYS_32BIT and __SYS_64BIT
 */
#if ( defined(__SYS_32BIT) && defined(__SYS_64BIT) )
#error "please don't define __SYS_32BIT and __SYS_64BIT at the same time";
#elif ( !defined(__SYS_32BIT) && !defined(__SYS_64BIT) )
#error "you must define __SYS_32BIT or __SYS_32BIT before continue";
#endif


#endif /* __XBOOT_H__ */
