#ifndef __K210_REG_SYSCTL_H__
#define __K210_REG_SYSCTL_H__

#define K210_SYSCTL_BASE	(0x50440000)

#define SYSCTL_GIT_ID		(0x00)
#define SYSCTL_CLK_FREQ		(0x04)
#define SYSCTL_PLL0			(0x08)
#define SYSCTL_PLL1			(0x0c)
#define SYSCTL_PLL2			(0x10)
#define SYSCTL_PLL_LOCK		(0x18)
#define SYSCTL_ROM_ERROR	(0x1c)
#define SYSCTL_CLK_SEL0		(0x20)
#define SYSCTL_CLK_SEL1		(0x24)
#define SYSCTL_CLK_EN_CENT	(0x28)
#define SYSCTL_CLK_EN_PERI	(0x2c)
#define SYSCTL_SOFT_RESET	(0x30)
#define SYSCTL_PERI_RESET	(0x34)
#define SYSCTL_CLK_TH0		(0x38)
#define SYSCTL_CLK_TH1		(0x3c)
#define SYSCTL_CLK_TH2		(0x40)
#define SYSCTL_CLK_TH3		(0x44)
#define SYSCTL_CLK_TH4		(0x48)
#define SYSCTL_CLK_TH5		(0x4c)
#define SYSCTL_CLK_TH6		(0x50)
#define SYSCTL_MISC			(0x54)
#define SYSCTL_PERI			(0x58)
#define SYSCTL_SPI_SLEEP	(0x5c)
#define SYSCTL_RESET_STATUS	(0x60)
#define SYSCTL_DMA_SEL0		(0x64)
#define SYSCTL_DMA_SEL1		(0x68)
#define SYSCTL_POWER_SEL	(0x6c)

#endif /* __K210_REG_SYSCTL_H__ */
