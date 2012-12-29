#ifndef __SERIAL_H__
#define __SERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
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
	/* the serial information */
	struct serial_info *info;

	/*initialize the serial */
	void (*init)(void);

	/* clean up the serial */
	void (*exit)(void);

	/* read buffer from serial */
	ssize_t (*read)(u8_t * buf, size_t count);

	/* write buffer to serial */
	ssize_t (*write)(const u8_t * buf, size_t count);

	/* ioctl serial */
	int (*ioctl)(int cmd, void * arg);
};

bool_t register_serial(struct serial_driver * drv);
bool_t unregister_serial(struct serial_driver * drv);

#ifdef __cplusplus
}
#endif

#endif /* __SERIAL_H__ */
