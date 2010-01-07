#ifndef __IOCTL_H__
#define __IOCTL_H__

#include <configs.h>
#include <default.h>

/* serial ioctl */
#define	IOCTL_WR_SERIAL_BAUD_RATE		(1)
#define	IOCTL_WR_SERIAL_DATA_BITS		(3)
#define	IOCTL_WR_SERIAL_PARITY_BIT		(5)
#define	IOCTL_WR_SERIAL_STOP_BITS		(7)

#define	IOCTL_RD_SERIAL_BAUD_RATE		(2)
#define	IOCTL_RD_SERIAL_DATA_BITS		(4)
#define	IOCTL_RD_SERIAL_PARITY_BIT		(6)
#define	IOCTL_RD_SERIAL_STOP_BITS		(8)


/* rtc ioctl */
#define	IOCTL_SET_RTC_TIME				(1)
#define	IOCTL_SET_RTC_ALARM				(3)

#define	IOCTL_GET_RTC_TIME				(2)
#define	IOCTL_GET_RTC_ALARM				(4)

#define	IOCTL_ENABLE_RTC_ALARM			(5)
#define	IOCTL_DISABLE_RTC_ALARM			(6)


#endif /* __IOCTL_H__ */
