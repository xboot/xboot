#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <types.h>
#include <string.h>
#include <time/xtime.h>

/*
 * defined the struct of rtc driver, which contains
 * low level operating fuction.
 */
struct rtc_driver
{
	/* the rtc name */
	char * name;

	/*initialize the rtc */
	void (*init)(void);

	/* clean up the rtc */
	void (*exit)(void);

	/* set rtc time */
	bool_t (*set_time)(struct time * time);

	/* get rtc time */
	bool_t (*get_time)(struct time * time);

	/* set rtc alarm */
	bool_t (*set_alarm)(struct time * time);

	/* get rtc alarm */
	bool_t (*get_alarm)(struct time * time);

	/* enable alarm or not */
	bool_t (*alarm_enable)(bool_t enable);
};


u32_t rtc_month_days(u32_t year, u32_t month);
u32_t rtc_year_days(u32_t year, u32_t month, u32_t day);
bool_t rtc_valid_time(struct time * tm);
void rtc_to_time(unsigned long time, struct time *tm);
u32_t time_to_rtc(struct time * tm);

bool_t register_rtc(struct rtc_driver * drv);
bool_t unregister_rtc(struct rtc_driver * drv);

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */
