#ifndef __IOCTL_H__
#define __IOCTL_H__

#include <configs.h>
#include <default.h>

/*
 * serial ioctl
 */
#define	IOCTL_WR_SERIAL_BAUD_RATE		(101)
#define	IOCTL_WR_SERIAL_DATA_BITS		(102)
#define	IOCTL_WR_SERIAL_PARITY_BIT		(103)
#define	IOCTL_WR_SERIAL_STOP_BITS		(104)
#define	IOCTL_RD_SERIAL_BAUD_RATE		(105)
#define	IOCTL_RD_SERIAL_DATA_BITS		(106)
#define	IOCTL_RD_SERIAL_PARITY_BIT		(107)
#define	IOCTL_RD_SERIAL_STOP_BITS		(108)

/*
 * rtc ioctl
 */
#define	IOCTL_SET_RTC_TIME				(111)
#define	IOCTL_SET_RTC_ALARM				(112)
#define	IOCTL_GET_RTC_TIME				(113)
#define	IOCTL_GET_RTC_ALARM				(114)
#define	IOCTL_ENABLE_RTC_ALARM			(115)
#define	IOCTL_DISABLE_RTC_ALARM			(116)

#endif /* __IOCTL_H__ */
