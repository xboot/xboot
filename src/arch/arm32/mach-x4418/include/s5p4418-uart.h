#ifndef __S5P4418_UART_H__
#define __S5P4418_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/uart.h>
#include <s5p4418-rstcon.h>
#include <s5p4418-gpio.h>
#include <s5p4418/reg-uart.h>

struct s5p4418_uart_data_t
{
	enum baud_rate_t baud;
	enum data_bits_t data;
	enum parity_bits_t parity;
	enum stop_bits_t stop;

	physical_addr_t regbase;
};

#ifdef __cplusplus
}
#endif

#endif /* __S5P4418_UART_H__ */
