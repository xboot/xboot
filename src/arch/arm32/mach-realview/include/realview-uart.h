#ifndef __REALVIEW_UART_H__
#define __REALVIEW_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/uart.h>
#include <realview/reg-uart.h>

struct realview_uart_t
{
	enum baud_rate_t baud;
	enum data_bits_t data;
	enum parity_bits_t parity;
	enum stop_bits_t stop;

	physical_addr_t base;
};

#ifdef __cplusplus
}
#endif

#endif /* __REALVIEW_UART_H__ */
