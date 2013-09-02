#ifndef __I2C_GPIO_H__
#define __I2C_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/i2c.h>
#include <bus/i2c-algo-bit.h>

struct i2c_gpio_data_t {
	int sda_pin;
	int scl_pin;
	int udelay;		/* us */
	int timeout;	/* ms */
	int sda_is_open_drain;
	int scl_is_open_drain;
	int scl_is_output_only;
};

#ifdef __cplusplus
}
#endif

#endif /* __I2C_GPIO_H__ */
