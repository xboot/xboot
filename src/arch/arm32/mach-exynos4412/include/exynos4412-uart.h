#ifndef __EXYNOS4412_UART_H__
#define __EXYNOS4412_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/uart.h>
#include <exynos4412/reg-gpio.h>
#include <exynos4412/reg-uart.h>

struct exynos4412_uart_data_t
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

#endif /* __EXYNOS4412_UART_H__ */
