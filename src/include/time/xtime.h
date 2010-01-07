#ifndef __XTIME_H__
#define __XTIME_H__


#include <configs.h>
#include <default.h>

struct time {
	x_u8 sec;
	x_u8 min;
	x_u8 hour;
	x_u8 day;
	x_u8 week;
	x_u8 mon;
	x_u16 year;
};


void do_system_xtime(void);
x_u32 mktime(const x_u32 year0, const x_u32 mon0, const x_u32 day, const x_u32 hour, const x_u32 min, const x_u32 sec);
x_u8 mkweek(x_u32 year, x_u32 mon, x_u32 day);
x_u32 get_time_stamp(void);

#endif /* __XTIME_H__ */
