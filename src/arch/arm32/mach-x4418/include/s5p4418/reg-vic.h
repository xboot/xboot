#ifndef __S5P4418_REG_VIC_H__
#define __S5P4418_REG_VIC_H__

#define S5P4418_VIC0_BASE			(0xC0002000)
#define S5P4418_VIC1_BASE			(0xC0003000)

#define VIC_IRQSTATUS				(0x000)
#define VIC_FIQSTATUS				(0x004)
#define VIC_RAWINTR					(0x008)
#define VIC_INTSELECT				(0x00c)
#define VIC_INTENABLE				(0x010)
#define VIC_INTENCLEAR				(0x014)
#define VIC_SOFTINT					(0x018)
#define VIC_SOFTINTCLEAR			(0x01c)
#define VIC_PROTECTION				(0x020)
#define VIC_SWPRIORITYMASK			(0x024)
#define VIC_PRIORITYDAISY			(0x028)
#define VIC_VECTADDR0				(0x100)
#define VIC_VECPRIORITY0			(0x200)
#define VIC_ADDRESS					(0xf00)
#define VIC_PERID0					(0xfe0)
#define VIC_PERID1					(0xfe4)
#define VIC_PERID2					(0xfe8)
#define VIC_PERID3					(0xfec)
#define VIC_PCELLID0				(0xff0)
#define VIC_PCELLID1				(0xff4)
#define VIC_PCELLID2				(0xff8)
#define VIC_PCELLID3				(0xffc)

#endif /* __S5P4418_REG_VIC_H__ */
