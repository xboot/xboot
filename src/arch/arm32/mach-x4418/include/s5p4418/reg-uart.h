#ifndef __S5P4418_REG_UART_H__
#define __S5P4418_REG_UART_H__

#define S5P4418_UART0_BASE		(0xC00A1000)
#define S5P4418_UART1_BASE		(0xC00A0000)
#define S5P4418_UART2_BASE		(0xC00A2000)
#define S5P4418_UART3_BASE		(0xC00A3000)

#define UART_DATA				(0x00)
#define UART_RSR				(0x04)
#define UART_FR					(0x18)
#define UART_ILPR				(0x20)
#define UART_IBRD				(0x24)
#define UART_FBRD				(0x28)
#define UART_LCRH				(0x2C)
#define UART_CR					(0x30)
#define UART_IFLS				(0x34)
#define UART_IMSC				(0x38)
#define UART_RIS				(0x3C)
#define UART_MIS				(0x40)
#define UART_ICR				(0x44)
#define UART_DMACR				(0x48)

#define UART_RSR_OE				(0x08)
#define UART_RSR_BE				(0x04)
#define UART_RSR_PE				(0x02)
#define UART_RSR_FE				(0x01)

#define UART_FR_RI				(0x100)
#define UART_FR_TXFE			(0x80)
#define UART_FR_RXFF			(0x40)
#define UART_FR_TXFF			(0x20)
#define UART_FR_RXFE			(0x10)
#define UART_FR_BUSY			(0x08)

#endif /* __S5P4418_REG_UART_H__ */
