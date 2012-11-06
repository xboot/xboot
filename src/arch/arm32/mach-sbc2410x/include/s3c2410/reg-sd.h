#ifndef __S3C2410_REG_SD_H__
#define __S3C2410_REG_SD_H__

#include <xboot.h>

#define S3C2410_SDICON	  		(0x5a000000)
#define S3C2410_SDIPRE	  		(0x5a000004)
#define S3C2410_SDICARG	  		(0x5a000008)
#define S3C2410_SDICCON	  		(0x5a00000c)
#define S3C2410_SDICSTA	  		(0x5a000010)

#define S3C2410_SDIRSP0	  		(0x5a000014)
#define S3C2410_SDIRSP1	  		(0x5a000018)
#define S3C2410_SDIRSP2	  		(0x5a00001c)
#define S3C2410_SDIRSP3	  		(0x5a000020)

#define S3C2410_SDIDTIMER	  	(0x5a000024)
#define S3C2410_SDIBSIZE	  	(0x5a000028)
#define S3C2410_SDIDCON	  		(0x5a00002c)
#define S3C2410_SDIDCNT			(0x5a000030)
#define S3C2410_SDIDSTA	  		(0x5a000034)
#define S3C2410_SDIFSTA	  		(0x5a000038)
#define S3C2410_SDIIMSK	  		(0x5a000040)

#if (BYTE_ORDER == BIG_ENDIAN)
#define S3C2410_SDIDAT	  		(0x5a00003f)
#else
#define S3C2410_SDIDAT	  		(0x5a00003c)
#endif


#endif /* __S3C2410_REG_SD_H__ */
