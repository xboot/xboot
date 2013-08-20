#ifndef __REALVIEW_REG_RTC_H__
#define __REALVIEW_REG_RTC_H__

#define REALVIEW_RTC_BASE			(0x10017000)

#define REALVIEW_RTC_OFFSET_DR		(0x00)	/* data read register */
#define REALVIEW_RTC_OFFSET_MR		(0x04)	/* match register */
#define REALVIEW_RTC_OFFSET_LR		(0x08)	/* data load register */
#define REALVIEW_RTC_OFFSET_CR		(0x0c)	/* control register */
#define REALVIEW_RTC_OFFSET_IMSC	(0x10)	/* interrupt mask and set register */
#define REALVIEW_RTC_OFFSET_RIS		(0x14)	/* raw interrupt status register */
#define REALVIEW_RTC_OFFSET_MIS		(0x18)	/* masked interrupt status register */
#define REALVIEW_RTC_OFFSET_ICR		(0x1c)	/* interrupt clear register */

#endif /* __REALVIEW_REG_RTC_H__ */
