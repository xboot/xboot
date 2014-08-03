#ifndef __BUS_UART_H__
#define __BUS_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum baud_rate_t {
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

enum data_bits_t {
	DATA_BITS_5,
	DATA_BITS_6,
	DATA_BITS_7,
	DATA_BITS_8,
};

enum parity_bits_t {
	PARITY_NONE,
	PARITY_EVEN,
	PARITY_ODD,
};

enum stop_bits_t {
	STOP_BITS_1,
	STOP_BITS_1_5,
	STOP_BITS_2,
};

struct uart_t
{
	/* The uart name */
	char * name;

	/* Initialize uart */
	void (*init)(struct uart_t * uart);

	/* Clean up uart */
	void (*exit)(struct uart_t * uart);

	/* read buffer */
	ssize_t (*read)(struct uart_t * uart, u8_t * buf, size_t count);

	/* write buffer */
	ssize_t (*write)(struct uart_t * uart, const u8_t * buf, size_t count);

	/* setup param */
	bool_t (*setup)(struct uart_t * uart, enum baud_rate_t baud, enum data_bits_t data, enum parity_bits_t parity, enum stop_bits_t stop);

	/* Private data */
	void * priv;
};

struct uart_t * search_bus_uart(const char * name);
bool_t register_bus_uart(struct uart_t * uart);
bool_t unregister_bus_uart(struct uart_t * uart);
bool_t uart_setup(struct uart_t * uart, enum baud_rate_t baud, enum data_bits_t data, enum parity_bits_t parity, enum stop_bits_t stop);
ssize_t uart_read(struct uart_t * uart, u8_t * buf, size_t count);
ssize_t uart_write(struct uart_t * uart, const u8_t * buf, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* __BUS_UART_H__ */
