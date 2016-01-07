#ifndef __BCM2836_UART_H__
#define __BCM2836_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/uart.h>
#include <bcm2836-gpio.h>
#include <bcm2836/reg-uart.h>

struct bcm2836_uart_data_t
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

#endif /* __BCM2836_UART_H__ */
