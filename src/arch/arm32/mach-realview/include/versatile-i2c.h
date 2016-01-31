#ifndef __VERSATILE_I2C_H__
#define __VERSATILE_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/i2c.h>
#include <bus/i2c-algo-bit.h>

struct versatile_i2c_data_t {
	int udelay;
	physical_addr_t phys;
};

#ifdef __cplusplus
}
#endif

#endif /* __VERSATILE_I2C_H__ */
