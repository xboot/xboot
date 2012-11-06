#ifndef __S3C2410_REG_IIS_H__
#define __S3C2410_REG_IIS_H__

#include <xboot.h>

#define S3C2410_IISCON	 		(0x55000000)
#define S3C2410_IISMOD	  		(0x58000004)
#define S3C2410_IISPSR	  		(0x55000008)
#define S3C2410_IISFCON	  		(0x5500000c)

#if (BYTE_ORDER == BIG_ENDIAN)
#define S3C2410_IISFIFO	  		(0x55000012)
#else
#define S3C2410_IISFIFO	  		(0x55000010)
#endif

#endif /* __S3C2410_REG_IIS_H__ */
