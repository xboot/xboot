#ifndef __REALVIEW_REG_RTC_H__
#define __REALVIEW_REG_RTC_H__

#define REALVIEW_RTC_BASE	(0x10017000)

#define RTC_DR				(0x00)	/* data read register */
#define RTC_MR				(0x04)	/* match register */
#define RTC_LR				(0x08)	/* data load register */
#define RTC_CR				(0x0c)	/* control register */
#define RTC_IMSC			(0x10)	/* interrupt mask and set register */
#define RTC_RIS				(0x14)	/* raw interrupt status register */
#define RTC_MIS				(0x18)	/* masked interrupt status register */
#define RTC_ICR				(0x1c)	/* interrupt clear register */

#endif /* __REALVIEW_REG_RTC_H__ */
