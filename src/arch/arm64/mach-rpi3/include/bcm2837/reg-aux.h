#ifndef __BCM2837_REG_AUX_H__
#define __BCM2837_REG_AUX_H__

#define BCM2837_AUX_BASE		(0x3f215000)
#define BCM2837_AUX_MU_BASE		(0x3f215040)
#define BCM2837_AUX_SPI0_BASE	(0x3f215080)
#define BCM2837_AUX_SPI1_BASE	(0x3f2150c0)

#define AUX_IRQ					(0x00)
#define AUX_ENB					(0x04)

#define AUX_MU_IO				(0x00)
#define AUX_MU_IER				(0x04)
#define AUX_MU_IIR				(0x08)
#define AUX_MU_LCR				(0x0c)
#define AUX_MU_MCR				(0x10)
#define AUX_MU_LSR				(0x14)
#define AUX_MU_MSR				(0x18)
#define AUX_MU_SCRATCH			(0x1c)
#define AUX_MU_CNTL				(0x20)
#define AUX_MU_STAT				(0x24)
#define AUX_MU_BAUD				(0x28)

#define AUX_SPI_CNTL0			(0x00)
#define AUX_SPI_CNTL1			(0x04)
#define AUX_SPI_STAT			(0x08)
#define AUX_SPI_IO				(0x10)
#define AUX_SPI_PEEK			(0x14)

#endif /* __BCM2837_REG_AUX_H__ */
