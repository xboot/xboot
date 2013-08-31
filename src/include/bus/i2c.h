#ifndef __BUS_I2C_H__
#define __BUS_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

#define I2C_M_TEN			0x0010	/* this is a ten bit chip address */
#define I2C_M_RD			0x0001	/* read data, from slave to master */
#define I2C_M_STOP			0x8000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NOSTART		0x4000	/* if I2C_FUNC_NOSTART */
#define I2C_M_REV_DIR_ADDR	0x2000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK	0x1000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NO_RD_ACK		0x0800	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_RECV_LEN		0x0400	/* length will be first received byte */

struct i2c_msg_t {
	u16_t addr;
	u16_t flags;
	u16_t len;
	u8_t * buf;
};

struct i2c_t
{
	/* The i2c bus name */
	char * name;

	/* Initialize i2c */
	void (*init)(struct i2c_t * i2c);

	/* Clean up i2c */
	void (*exit)(struct i2c_t * i2c);

	/* I2C master xfer */
	int (*xfer)(struct i2c_t * i2c, struct i2c_msg_t * msgs, int num);

	/* Private data */
	void * priv;
};

struct i2c_t * search_bus_i2c(const char * name);
bool_t register_bus_i2c(struct i2c_t * i2c);
bool_t unregister_bus_i2c(struct i2c_t * i2c);

#ifdef __cplusplus
}
#endif

#endif /* __BUS_I2C_H__ */
