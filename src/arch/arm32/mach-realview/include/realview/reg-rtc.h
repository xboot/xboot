#ifndef __REALVIEW_REG_RTC_H__
#define __REALVIEW_REG_RTC_H__

#include <xboot.h>

/*
 * pl031 register definitions
 */
#define REALVIEW_RTC_DR				(0x10017000 + 0x00)		/* data read register */
#define REALVIEW_RTC_MR				(0x10017000 + 0x04)		/* match register */
#define REALVIEW_RTC_LR				(0x10017000 + 0x08)		/* data load register */
#define REALVIEW_RTC_CR				(0x10017000 + 0x0c)		/* control register */
#define REALVIEW_RTC_IMSC			(0x10017000 + 0x10)		/* interrupt mask and set register */
#define REALVIEW_RTC_RIS			(0x10017000 + 0x14)		/* raw interrupt status register */
#define REALVIEW_RTC_MIS			(0x10017000 + 0x18)		/* masked interrupt status register */
#define REALVIEW_RTC_ICR			(0x10017000 + 0x1c)		/* interrupt clear register */


#endif /* __REALVIEW_REG_RTC_H__ */
