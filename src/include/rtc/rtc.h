#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct rtc_time_t {
	u8_t second;	/* second [0 - 59] */
	u8_t minute;	/* minute [0 - 59] */
	u8_t hour;		/* hour [0 - 23] */
	u8_t week;		/* week [0 - 6] */
	u8_t day;		/* day [1 - 31] */
	u8_t month;		/* month [1 - 12] */
	u32_t year;		/* year */
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

void secs_to_rtc_time(uint64_t time, struct rtc_time_t * rt);
uint64_t rtc_time_to_secs(struct rtc_time_t * rt);
int rtc_time_is_valid(struct rtc_time_t * time);

struct rtc_t * search_rtc(const char * name);
struct rtc_t * search_first_rtc(void);
struct device_t * register_rtc(struct rtc_t * rtc, struct driver_t * drv);
void unregister_rtc(struct rtc_t * rtc);

bool_t rtc_settime(struct rtc_t * rtc, struct rtc_time_t * time);
bool_t rtc_gettime(struct rtc_t * rtc, struct rtc_time_t * time);

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */
