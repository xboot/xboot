#ifndef __VIRT_REG_UART_H__
#define __VIRT_REG_UART_H__

#define VIRT_UART0_BASE		(0x09000000)

#define UART_DATA			(0x00)
#define UART_RSR			(0x04)
#define UART_FR				(0x18)
#define UART_IBRD			(0x24)
#define UART_FBRD			(0x28)
#define UART_LCRH			(0x2C)
#define UART_CR				(0x30)
#define UART_IMSC			(0x38)

#define UART_RSR_OE			(0x08)
#define UART_RSR_BE			(0x04)
#define UART_RSR_PE			(0x02)
#define UART_RSR_FE			(0x01)

#define UART_FR_TXFE		(0x80)
#define UART_FR_RXFF		(0x40)
#define UART_FR_TXFF		(0x20)
#define UART_FR_RXFE		(0x10)
#define UART_FR_BUSY		(0x08)

#endif /* __VIRT_REG_UART_H__ */
