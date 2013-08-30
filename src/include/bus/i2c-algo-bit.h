#ifndef __I2C_ALGO_BIT_H__
#define __I2C_ALGO_BIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/i2c.h>

struct i2c_algo_bit_data_t {
	void (*setsda)(struct i2c_algo_bit_data_t * data, int state);
	void (*setscl)(struct i2c_algo_bit_data_t * data, int state);
	int (*getsda)(struct i2c_algo_bit_data_t * data);
	int (*getscl)(struct i2c_algo_bit_data_t * data);
	int (*pre_xfer)(struct i2c_t *);
	void (*post_xfer)(struct i2c_t *);

	/* local settings */
	int udelay;		/* half clock cycle time in us,
				   minimum 2 us for fast-mode I2C,
				   minimum 5 us for standard-mode I2C and SMBus,
				   maximum 50 us for SMBus */
	int timeout;		/* in jiffies */

	/* Private data */
	void * data;
};

#ifdef __cplusplus
}
#endif

#endif /* __I2C_ALGO_BIT_H__ */
