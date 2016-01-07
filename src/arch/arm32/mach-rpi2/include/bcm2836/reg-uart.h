#ifndef __BCM2836_REG_UART_H__
#define __BCM2836_REG_UART_H__

#define BCM2836_UART0_BASE	(0x3f201000)

#define UART_DATA			(0x00)
#define UART_RSR			(0x04)
#define UART_FR				(0x18)
#define UART_IBRD			(0x24)
#define UART_FBRD			(0x28)
#define UART_LCRH			(0x2c)
#define UART_CR				(0x30)
#define UART_IFLS			(0x34)
#define UART_IMSC			(0x38)
#define UART_RIS			(0x3c)
#define UART_MIS			(0x40)
#define UART_ICR			(0x44)
#define UART_DMACR			(0x48)

#define UART_RSR_OE			(0x1 << 3)
#define UART_RSR_BE			(0x1 << 2)
#define UART_RSR_PE			(0x1 << 1)
#define UART_RSR_FE			(0x1 << 0)

#define UART_FR_TXFE		(0x1 << 7)
#define UART_FR_RXFF		(0x1 << 6)
#define UART_FR_TXFF		(0x1 << 5)
#define UART_FR_RXFE		(0x1 << 4)
#define UART_FR_BUSY		(0x1 << 3)

#endif /* __BCM2836_REG_UART_H__ */
