#ifndef __BUS_UART_H__
#define __BUS_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct uart_t
{
	/* The uart name */
	char * name;

	/* Set uart param */
	bool_t (*set)(struct uart_t * uart, int baud, int data, int parity, int stop);

	/* Get uart param */
	bool_t (*get)(struct uart_t * uart, int * baud, int * data, int * parity, int * stop);

	/* Read uart */
	ssize_t (*read)(struct uart_t * uart, u8_t * buf, size_t count);

	/* Write uart */
	ssize_t (*write)(struct uart_t * uart, const u8_t * buf, size_t count);

	/* Private data */
	void * priv;
};

struct uart_t * search_uart(const char * name);
struct device_t * register_uart(struct uart_t * uart, struct driver_t * drv);
void unregister_uart(struct uart_t * uart);

bool_t uart_set(struct uart_t * uart, int baud, int data, int parity, int stop);
bool_t uart_get(struct uart_t * uart, int * baud, int * data, int * parity, int * stop);
ssize_t uart_read(struct uart_t * uart, u8_t * buf, size_t count);
ssize_t uart_write(struct uart_t * uart, const u8_t * buf, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* __BUS_UART_H__ */
