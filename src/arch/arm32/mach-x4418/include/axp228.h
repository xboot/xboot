#ifndef __AXP228_H__
#define __AXP228_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/i2c.h>
#include <battery/battery.h>

struct axp228_data_t
{
	const char * i2cbus;
	u32_t addr;
};

#ifdef __cplusplus
}
#endif

#endif /* __AXP228_H__ */
