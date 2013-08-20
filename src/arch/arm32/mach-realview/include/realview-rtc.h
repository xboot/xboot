#ifndef __REALVIEW_RTC_H__
#define __REALVIEW_RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <rtc/rtc.h>
#include <realview/reg-rtc.h>

struct realview_rtc_data_t
{
	physical_addr_t regbase;
};

#ifdef __cplusplus
}
#endif

#endif /* __REALVIEW_RTC_H__ */
