#ifndef __RK3288_REG_SPDIF_H__
#define __RK3288_REG_SPDIF_H__

#define RK3288_SPDIF_2CH_BASE	(0xff880000)
#define RK3288_SPDIF_8CH_BASE	(0xff8b0000)

#define SPDIF_CFGR				(0x0000)
#define SPDIF_SDBLR				(0x0004)
#define SPDIF_DMACR				(0x0008)
#define SPDIF_INTCR				(0x000c)
#define SPDIF_INTSR				(0x0010)
#define SPDIF_XFER				(0x0018)
#define SPDIF_SMPDR				(0x0020)
#define SPDIF_VLDFR				(0x0060)
#define SPDIF_USRDR				(0x0090)
#define SPDIF_CHNSR				(0x00c0)
#define SPDIF_BURTSINFO			(0x0100)
#define SPDIF_REPETTION			(0x0104)
#define SPDIF_BURTSINFO_SHD		(0x0108)
#define SPDIF_REPETTION_SHD		(0x010c)
#define SPDIF_USRDR_SHD			(0x0190)

#endif /* __RK3288_REG_SPDIF_H__ */
