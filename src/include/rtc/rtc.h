#ifndef __RTC_H__
#define __RTC_H__


#include <configs.h>
#include <default.h>
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
	x_bool (*set_time)(struct time * time);

	/* get rtc time */
	x_bool (*get_time)(struct time * time);

	/* set rtc alarm */
	x_bool (*set_alarm)(struct time * time);

	/* get rtc alarm */
	x_bool (*get_alarm)(struct time * time);

	/* enable alarm or not */
	x_bool (*alarm_enable)(x_bool enable);

	/* driver device */
	void * device;
};


x_u32 rtc_month_days(x_u32 year, x_u32 month);
x_u32 rtc_year_days(x_u32 year, x_u32 month, x_u32 day);
x_bool rtc_valid_time(struct time * tm);
void rtc_to_time(unsigned long time, struct time *tm);
x_u32 time_to_rtc(struct time * tm);

x_bool register_rtc(struct rtc_driver * drv);
x_bool unregister_rtc(struct rtc_driver * drv);


#endif /* __RTC_H__ */
