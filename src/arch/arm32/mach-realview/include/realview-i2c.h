#ifndef __REALVIEW_I2C_H__
#define __REALVIEW_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/i2c.h>
#include <bus/i2c-algo-bit.h>
#include <realview/reg-i2c.h>

struct realview_i2c_data_t {
	int udelay;
	int timeout;
	physical_addr_t regbase;
};

#ifdef __cplusplus
}
#endif

#endif /* __REALVIEW_I2C_H__ */
