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

	/* Initialize the rtc */
	void (*init)(struct rtc_t * rtc);

	/* Clean up the rtc */
	void (*exit)(struct rtc_t * rtc);

	/* Set rtc time */
	bool_t (*settime)(struct rtc_t * rtc, struct rtc_time_t * time);

	/* Get rtc time */
	bool_t (*gettime)(struct rtc_t * rtc, struct rtc_time_t * time);

	/* Suspend rtc */
	void (*suspend)(struct rtc_t * rtc);

	/* Resume rtc */
	void (*resume)(struct rtc_t * rtc);

	/* Private data */
	void * priv;
};

struct rtc_t * search_rtc(const char * name);
bool_t register_rtc(struct rtc_t * rtc);
bool_t unregister_rtc(struct rtc_t * rtc);

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */
