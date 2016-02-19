#ifndef __GSLX680_H__
#define __GSLX680_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/i2c.h>
#include <input/input.h>

struct gslx680_ts_firmware_t {
	u8_t reg;
	u32_t val;
};

struct gslx680_ts_data_t
{
	const char * i2cbus;
	int addr;
	int irq;
	int shutdown;
	int fingers;
	const struct gslx680_ts_firmware_t * firmware;
};

#ifdef __cplusplus
}
#endif

#endif /* __GSLX680_H__ */
