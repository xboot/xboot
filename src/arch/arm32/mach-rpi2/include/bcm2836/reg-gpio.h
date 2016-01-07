#ifndef __BCM2836_REG_GPIO_H__
#define __BCM2836_REG_GPIO_H__

#define BCM2836_GPIO_BASE	(0x3f200000)

#define GPIO_FSEL(x)		(0x00 + (x) * 4)
#define GPIO_SET(x)			(0x1c + (x) * 4)
#define GPIO_CLR(x)			(0x28 + (x) * 4)
#define GPIO_LEV(x)			(0x34 + (x) * 4)
#define GPIO_EDS(x)			(0x40 + (x) * 4)
#define GPIO_REN(x)			(0x4c + (x) * 4)
#define GPIO_FEN(x)			(0x58 + (x) * 4)
#define GPIO_HEN(x)			(0x64 + (x) * 4)
#define GPIO_LEN(x)			(0x70 + (x) * 4)
#define GPIO_AREN(x)		(0x7c + (x) * 4)
#define GPIO_AFEN(x)		(0x88 + (x) * 4)
#define GPIO_UD(x)			(0x94 + (x) * 4)
#define GPIO_UDCLK(x)		(0x98 + (x) * 4)

#endif /* __BCM2836_REG_GPIO_H__ */
