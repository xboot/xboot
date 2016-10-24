#ifndef __RK3399_REG_GRF_H__
#define __RK3399_REG_GRF_H__

#define RK3399_GRF_BASE			(0xff770000)

#define GRF_GPIO2A_IOMUX		(0xe000)
#define GRF_GPIO2B_IOMUX		(0xe004)
#define GRF_GPIO2C_IOMUX		(0xe008)
#define GRF_GPIO2D_IOMUX		(0xe00c)
#define GRF_GPIO3A_IOMUX		(0xe010)
#define GRF_GPIO3B_IOMUX		(0xe014)
#define GRF_GPIO3C_IOMUX		(0xe018)
#define GRF_GPIO3D_IOMUX		(0xe01c)
#define GRF_GPIO4A_IOMUX		(0xe020)
#define GRF_GPIO4B_IOMUX		(0xe024)
#define GRF_GPIO4C_IOMUX		(0xe028)
#define GRF_GPIO4D_IOMUX		(0xe02c)

#define GRF_GPIO2A_P			(0xe040)
#define GRF_GPIO2B_P			(0xe044)
#define GRF_GPIO2C_P			(0xe048)
#define GRF_GPIO2D_P			(0xe04c)
#define GRF_GPIO3A_P			(0xe050)
#define GRF_GPIO3B_P			(0xe054)
#define GRF_GPIO3C_P			(0xe058)
#define GRF_GPIO3D_P			(0xe05c)
#define GRF_GPIO4A_P			(0xe060)
#define GRF_GPIO4B_P			(0xe064)
#define GRF_GPIO4C_P			(0xe068)
#define GRF_GPIO4D_P			(0xe06c)

#define GRF_CPU_CON0			(0xa000)
#define GRF_CPU_CON1			(0xa004)
#define GRF_CPU_CON2			(0xa008)
#define GRF_CPU_CON3			(0xa00c)

#define GRF_CPU_STATUS0			(0xa080)
#define GRF_CPU_STATUS1			(0xa084)
#define GRF_CPU_STATUS2			(0xa088)
#define GRF_CPU_STATUS3			(0xa08c)
#define GRF_CPU_STATUS4			(0xa090)
#define GRF_CPU_STATUS5			(0xa094)

#define GRF_SOC_CON0			(0xe200)
#define GRF_SOC_CON1			(0xe204)
#define GRF_SOC_CON2			(0xe208)
#define GRF_SOC_CON3			(0xe20c)
#define GRF_SOC_CON4			(0xe210)
#define GRF_SOC_CON5			(0xe214)
#define GRF_SOC_CON7			(0xe21c)
#define GRF_SOC_CON8			(0xe220)
#define GRF_SOC_CON9			(0xe224)

#define GRF_SOC_STATUS0			(0xe2a0)
#define GRF_SOC_STATUS1			(0xe2a4)
#define GRF_SOC_STATUS2			(0xe2a8)
#define GRF_SOC_STATUS3			(0xe2ac)
#define GRF_SOC_STATUS4			(0xe2b0)
#define GRF_SOC_STATUS5			(0xe2b4)

#define GRF_USB20PHY0_CON(i)	(0xe450 + ((i) * 4))
#define GRF_USB20PHY1_CON(i)	(0xe460 + ((i) * 4))

#define GRF_USB3PHY0_CON(i)		(0xe580 + ((i) * 4))
#define GRF_USB3PHY1_CON(i)		(0xe58c + ((i) * 4))
#define GRF_USB3PHY_STATUS(i)	(0xe5c0 + ((i) * 4))

#define GRF_IO_VSEL				(0xe640)
#define GRF_CHIP_ID_ADDR		(0xe800)

#define GRF_EMMCCORE_CON(i)		(0xf000 + ((i) * 4))
#define GRF_EMMCCORE_STATUS(i)	(0xf040 + ((i) * 4))
#define GRF_EMMCPHY_CON(i)		(0xf780 + ((i) * 4))
#define GRF_EMMCPHY_STATUS		(0xf7a0)

#define GRF_USB3OTG0_CON0		(0x2430)
#define GRF_SIG_DETECT_CLR		(0xe3d0)
#define GRF_SIG_DETECT_STATUS	(0xe3e0)

#endif /* __RK3399_REG_GRF_H__ */
