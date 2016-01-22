#ifndef __PL011_UART_H__
#define __PL011_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/uart.h>

struct pl011_uart_data_t
{
	char * clk;
	int txdpin;
	int txdcfg;
	int rxdpin;
	int rxdcfg;
	enum baud_rate_t baud;
	enum data_bits_t data;
	enum parity_bits_t parity;
	enum stop_bits_t stop;
	physical_addr_t regbase;
};

#ifdef __cplusplus
}
#endif

#endif /* __PL011_UART_H__ */
