#ifndef __SERIAL_H__
#define __SERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum serial_baud_rate_t {
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

enum serial_data_bits_t {
	DATA_BITS_5,
	DATA_BITS_6,
	DATA_BITS_7,
	DATA_BITS_8,
};

enum serial_parity_bits_t {
	PARITY_NONE,
	PARITY_EVEN,
	PARITY_ODD,
};

enum serial_stop_bits_t {
	STOP_BITS_1,
	STOP_BITS_1_5,
	STOP_BITS_2,
};

struct serial_parameter_t
{
	enum serial_baud_rate_t		baud_rate;
	enum serial_data_bits_t		data_bit;
	enum serial_parity_bits_t	parity;
	enum serial_stop_bits_t		stop_bit;
};

struct serial_info_t
{
	/* the serial name. */
	const char * name;

	/* the serial description */
	const char * desc;

	/* serial parameter */
	struct serial_parameter_t * parameter;
};

struct serial_driver_t
{
	/* the serial information */
	struct serial_info_t * info;

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

bool_t register_serial(struct serial_driver_t * drv);
bool_t unregister_serial(struct serial_driver_t * drv);

#ifdef __cplusplus
}
#endif

#endif /* __SERIAL_H__ */
