#ifndef __I2C_ALGO_BIT_H__
#define __I2C_ALGO_BIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <i2c/i2c.h>

struct i2c_algo_bit_data_t {
	void (*setsda)(struct i2c_algo_bit_data_t * data, int state);
	void (*setscl)(struct i2c_algo_bit_data_t * data, int state);
	int (*getsda)(struct i2c_algo_bit_data_t * data);
	int (*getscl)(struct i2c_algo_bit_data_t * data);
	/*
	 * Half clock cycle time in us
	 * minimum 2 us for fast-mode I2C - 400khz
	 * minimum 5 us for standard-mode I2C and SMBus - 100khz
	 * maximum 50 us for SMBus - 10khz
	 */
	int udelay;
	void * priv;
};

int i2c_algo_bit_xfer(struct i2c_algo_bit_data_t * bdat, struct i2c_msg_t * msgs, int num);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_ALGO_BIT_H__ */
