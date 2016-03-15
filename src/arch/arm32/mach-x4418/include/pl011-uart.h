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
	int irq;
	int fifosz;
	int txdpin;
	int txdcfg;
	int rxdpin;
	int rxdcfg;
	int baud;
	int data;
	int parity;
	int stop;
	physical_addr_t phys;
};

#ifdef __cplusplus
}
#endif

#endif /* __PL011_UART_H__ */
