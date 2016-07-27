#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct rtc_time_t {
	u8_t sec;
	u8_t min;
	u8_t hour;
	u8_t day;
	u8_t week;
	u8_t mon;
	u16_t year;
};

struct rtc_t
{
	/* The rtc name */
	char * name;

	/* Set rtc time */
	bool_t (*settime)(struct rtc_t * rtc, struct rtc_time_t * time);

	/* Get rtc time */
	bool_t (*gettime)(struct rtc_t * rtc, struct rtc_time_t * time);

	/* Private data */
	void * priv;
};

struct rtc_t * search_rtc(const char * name);
struct rtc_t * search_first_rtc(void);
bool_t register_rtc(struct device_t ** device, struct rtc_t * rtc);
bool_t unregister_rtc(struct rtc_t * rtc);

bool_t rtc_settime(struct rtc_t * rtc, struct rtc_time_t * time);
bool_t rtc_gettime(struct rtc_t * rtc, struct rtc_time_t * time);

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */
