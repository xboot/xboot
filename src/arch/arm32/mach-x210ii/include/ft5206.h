#ifndef __FT5206_H__
#define __FT5206_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/i2c.h>
#include <input/input.h>
#include <s5pv210/reg-gpio.h>

struct ft5206_data_t
{
	const char * i2cbus;
	u32_t addr;
	int rst_pin;
};

#ifdef __cplusplus
}
#endif

#endif /* __FT5206_H__ */
