#ifndef __CONSOLE_UART_H__
#define __CONSOLE_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <uart/uart.h>

bool_t register_console_uart(struct uart_t * uart);
bool_t unregister_console_uart(struct uart_t * uart);

#ifdef __cplusplus
}
#endif

#endif /* __CONSOLE_UART_H__ */
