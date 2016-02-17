#ifndef __BUS_I2C_H__
#define __BUS_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum {
	I2C_M_TEN			= 0x0010,
	I2C_M_RD			= 0x0001,
	I2C_M_STOP			= 0x8000,
	I2C_M_NOSTART		= 0x4000,
	I2C_M_REV_DIR_ADDR	= 0x2000,
	I2C_M_IGNORE_NAK	= 0x1000,
	I2C_M_NO_RD_ACK		= 0x0800,
	I2C_M_RECV_LEN		= 0x0400,
};

struct i2c_msg_t {
	int addr;
	int flags;
	int len;
	void * buf;
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

struct i2c_client_t {
	struct i2c_t * i2c;
	int addr;
	int flags;
};

struct i2c_t * search_bus_i2c(const char * name);
bool_t register_bus_i2c(struct i2c_t * i2c);
bool_t unregister_bus_i2c(struct i2c_t * i2c);

struct i2c_client_t * i2c_client_alloc(const char * i2cbus, int addr, int flags);
void i2c_client_free(struct i2c_client_t * client);

int i2c_transfer(struct i2c_t * i2c, struct i2c_msg_t * msgs, int num);
int i2c_master_send(const struct i2c_client_t * client, void * buf, int count);
int i2c_master_recv(const struct i2c_client_t * client, void * buf, int count);

#ifdef __cplusplus
}
#endif

#endif /* __BUS_I2C_H__ */
