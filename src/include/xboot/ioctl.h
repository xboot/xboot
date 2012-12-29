#ifndef __IOCTL_H__
#define __IOCTL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

/*
 * ioctl command
 */
enum
{
	/*
	 * serial ioctl
	 */
	IOCTL_WR_SERIAL_BAUD_RATE			= 101,
	IOCTL_WR_SERIAL_DATA_BITS			= 102,
	IOCTL_WR_SERIAL_PARITY_BIT			= 103,
	IOCTL_WR_SERIAL_STOP_BITS			= 104,
	IOCTL_RD_SERIAL_BAUD_RATE			= 105,
	IOCTL_RD_SERIAL_DATA_BITS			= 106,
	IOCTL_RD_SERIAL_PARITY_BIT			= 107,
	IOCTL_RD_SERIAL_STOP_BITS			= 108,

	/*
	 * rtc ioctl
	 */
	IOCTL_SET_RTC_TIME					= 111,
	IOCTL_SET_RTC_ALARM					= 112,
	IOCTL_GET_RTC_TIME					= 113,
	IOCTL_GET_RTC_ALARM					= 114,
	IOCTL_ENABLE_RTC_ALARM				= 115,
	IOCTL_DISABLE_RTC_ALARM				= 116,

	/*
	 * fb ioctl
	 */
	IOCTL_SET_FB_BACKLIGHT				= 121,
	IOCTL_GET_FB_BACKLIGHT				= 122,
};

#ifdef __cplusplus
}
#endif

#endif /* __IOCTL_H__ */
