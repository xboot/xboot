#ifndef __UART_CONSOLE_H__
#define __UART_CONSOLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/uart.h>

bool_t register_uart_console(struct uart_t * uart);
bool_t unregister_uart_console(struct uart_t * uart);

#ifdef __cplusplus
}
#endif

#endif /* __UART_CONSOLE_H__ */
