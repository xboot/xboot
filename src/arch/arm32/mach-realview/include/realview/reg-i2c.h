#ifndef __REALVIEW_REG_I2C_H__
#define __REALVIEW_REG_I2C_H__

#define REALVIEW_I2C_BASE	(0x10002000)

#define I2C_CTRL			(0x00)
#define I2C_CTRLS			(0x00)
#define I2C_CTRLC			(0x04)

#define I2C_FLAG_SCL		(1 << 0)
#define I2C_FLAG_SDA		(1 << 1)

#endif /* __REALVIEW_REG_I2C_H__ */
