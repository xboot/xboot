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
	int udelay;
	int timeout;
	void * data;
	void * priv;
};

#ifdef __cplusplus
}
#endif

#endif /* __I2C_ALGO_BIT_H__ */
