#ifndef __XTIME_H__
#define __XTIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct time {
	u8_t sec;
	u8_t min;
	u8_t hour;
	u8_t day;
	u8_t week;
	u8_t mon;
	u16_t year;
};

void do_system_xtime(void);
u32_t xmktime(const u32_t year0, const u32_t mon0, const u32_t day, const u32_t hour, const u32_t min, const u32_t sec);
u8_t xmkweek(u32_t year, u32_t mon, u32_t day);
u32_t get_time_stamp(void);

#ifdef __cplusplus
}
#endif

#endif /* __XTIME_H__ */
