#ifndef __PL031_RTC_H__
#define __PL031_RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <rtc/rtc.h>

struct pl031_rtc_data_t
{
	physical_addr_t regbase;
};

#ifdef __cplusplus
}
#endif

#endif /* __PL031_RTC_H__ */
