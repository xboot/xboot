#ifndef __SERIAL_H__
#define __SERIAL_H__


#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>

/**
 * the baud rate.
 */
enum SERIAL_BAUD_RATE {
	B50,
	B75,
	B110,
	B134,
	B200,
	B300,
	B600,
	B1200,
	B1800,
	B2400,
	B4800,
	B9600,
	B19200,
	B38400,
	B57600,
	B76800,
	B115200,
	B230400,
	B380400,
	B460800,
	B921600
};

/**
 * the data bits
 */
enum SERIAL_DATA_BITS {
	DATA_BITS_5,
	DATA_BITS_6,
	DATA_BITS_7,
	DATA_BITS_8,
};

/**
 * the parity bits
 */
enum SERIAL_PARITY_BIT {
	PARITY_NONE,
	PARITY_EVEN,
	PARITY_ODD,
};

/**
 * the stop bits
 */
enum SERIAL_STOP_BITS {
	STOP_BITS_1,
	STOP_BITS_1_5,
	STOP_BITS_2,
};

/**
 * serial parameter
 */
struct serial_parameter
{
	enum SERIAL_BAUD_RATE 	baud_rate;
	enum SERIAL_DATA_BITS 	data_bit;
	enum SERIAL_PARITY_BIT	parity;
	enum SERIAL_STOP_BITS  	stop_bit;
};

/**
 * defined the struct of serial information.
 */
struct serial_info
{
	/* the serial name. */
	const char *name;

	/* the serial description */
	const char *desc;

	/* serial parameter */
	struct serial_parameter *parameter;
};

/**
 * defined the struct of serial driver, which contains
 * low level operating fuction.
 */
struct serial_driver
{
	/* the serial infomation */
	struct serial_info *info;

	/*initialize the serial */
	void (*init)(void);

	/* clean up the serial */
	void (*exit)(void);

	/* read buffer from serial */
	x_s32 (*read)(x_u8 * buf, x_s32 count);

	/* write buffer to serial */
	x_s32 (*write)(const x_u8 * buf, x_s32 count);

	/* ioctl serial */
	x_s32 (*ioctl)(x_u32 cmd, void * arg);
};

x_bool register_serial(struct serial_driver * drv);
x_bool unregister_serial(struct serial_driver * drv);


#endif /* __SERIAL_H__ */
