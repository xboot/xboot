#ifndef __K210_GPIO_H__
#define __K210_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	CFG_JTAG_TCLK		= 0,   /* JTAG Test Clock */
	CFG_JTAG_TDI		= 1,   /* JTAG Test Data In */
	CFG_JTAG_TMS		= 2,   /* JTAG Test Mode Select */
	CFG_JTAG_TDO		= 3,   /* JTAG Test Data Out */
	CFG_SPI0_D0			= 4,   /* SPI0 Data 0 */
	CFG_SPI0_D1			= 5,   /* SPI0 Data 1 */
	CFG_SPI0_D2			= 6,   /* SPI0 Data 2 */
	CFG_SPI0_D3			= 7,   /* SPI0 Data 3 */
	CFG_SPI0_D4			= 8,   /* SPI0 Data 4 */
	CFG_SPI0_D5			= 9,   /* SPI0 Data 5 */
	CFG_SPI0_D6			= 10,  /* SPI0 Data 6 */
	CFG_SPI0_D7			= 11,  /* SPI0 Data 7 */
	CFG_SPI0_SS0		= 12,  /* SPI0 Chip Select 0 */
	CFG_SPI0_SS1		= 13,  /* SPI0 Chip Select 1 */
	CFG_SPI0_SS2		= 14,  /* SPI0 Chip Select 2 */
	CFG_SPI0_SS3		= 15,  /* SPI0 Chip Select 3 */
	CFG_SPI0_ARB		= 16,  /* SPI0 Arbitration */
	CFG_SPI0_SCLK		= 17,  /* SPI0 Serial Clock */
	CFG_UARTHS_RX		= 18,  /* UART High Speed Receiver */
	CFG_UARTHS_TX		= 19,  /* UART High Speed Transmitter */
	CFG_CLK_IN1			= 20,  /* Clock Input 1 */
	CFG_CLK_IN2			= 21,  /* Clock Input 2 */
	CFG_CLK_SPI1		= 22,  /* Clock SPI1 */
	CFG_CLK_I2C1		= 23,  /* Clock I2C1 */
	CFG_GPIOHS0			= 24,  /* GPIO High Speed 0 */
	CFG_GPIOHS1			= 25,  /* GPIO High Speed 1 */
	CFG_GPIOHS2			= 26,  /* GPIO High Speed 2 */
	CFG_GPIOHS3			= 27,  /* GPIO High Speed 3 */
	CFG_GPIOHS4			= 28,  /* GPIO High Speed 4 */
	CFG_GPIOHS5			= 29,  /* GPIO High Speed 5 */
	CFG_GPIOHS6			= 30,  /* GPIO High Speed 6 */
	CFG_GPIOHS7			= 31,  /* GPIO High Speed 7 */
	CFG_GPIOHS8			= 32,  /* GPIO High Speed 8 */
	CFG_GPIOHS9			= 33,  /* GPIO High Speed 9 */
	CFG_GPIOHS10		= 34,  /* GPIO High Speed 10 */
	CFG_GPIOHS11		= 35,  /* GPIO High Speed 11 */
	CFG_GPIOHS12		= 36,  /* GPIO High Speed 12 */
	CFG_GPIOHS13		= 37,  /* GPIO High Speed 13 */
	CFG_GPIOHS14		= 38,  /* GPIO High Speed 14 */
	CFG_GPIOHS15		= 39,  /* GPIO High Speed 15 */
	CFG_GPIOHS16		= 40,  /* GPIO High Speed 16 */
	CFG_GPIOHS17		= 41,  /* GPIO High Speed 17 */
	CFG_GPIOHS18		= 42,  /* GPIO High Speed 18 */
	CFG_GPIOHS19		= 43,  /* GPIO High Speed 19 */
	CFG_GPIOHS20		= 44,  /* GPIO High Speed 20 */
	CFG_GPIOHS21		= 45,  /* GPIO High Speed 21 */
	CFG_GPIOHS22		= 46,  /* GPIO High Speed 22 */
	CFG_GPIOHS23		= 47,  /* GPIO High Speed 23 */
	CFG_GPIOHS24		= 48,  /* GPIO High Speed 24 */
	CFG_GPIOHS25		= 49,  /* GPIO High Speed 25 */
	CFG_GPIOHS26		= 50,  /* GPIO High Speed 26 */
	CFG_GPIOHS27		= 51,  /* GPIO High Speed 27 */
	CFG_GPIOHS28		= 52,  /* GPIO High Speed 28 */
	CFG_GPIOHS29		= 53,  /* GPIO High Speed 29 */
	CFG_GPIOHS30		= 54,  /* GPIO High Speed 30 */
	CFG_GPIOHS31		= 55,  /* GPIO High Speed 31 */
	CFG_GPIO0			= 56,  /* GPIO pin 0 */
	CFG_GPIO1			= 57,  /* GPIO pin 1 */
	CFG_GPIO2			= 58,  /* GPIO pin 2 */
	CFG_GPIO3			= 59,  /* GPIO pin 3 */
	CFG_GPIO4			= 60,  /* GPIO pin 4 */
	CFG_GPIO5			= 61,  /* GPIO pin 5 */
	CFG_GPIO6			= 62,  /* GPIO pin 6 */
	CFG_GPIO7			= 63,  /* GPIO pin 7 */
	CFG_UART1_RX		= 64,  /* UART1 Receiver */
	CFG_UART1_TX		= 65,  /* UART1 Transmitter */
	CFG_UART2_RX		= 66,  /* UART2 Receiver */
	CFG_UART2_TX		= 67,  /* UART2 Transmitter */
	CFG_UART3_RX		= 68,  /* UART3 Receiver */
	CFG_UART3_TX		= 69,  /* UART3 Transmitter */
	CFG_SPI1_D0			= 70,  /* SPI1 Data 0 */
	CFG_SPI1_D1			= 71,  /* SPI1 Data 1 */
	CFG_SPI1_D2			= 72,  /* SPI1 Data 2 */
	CFG_SPI1_D3			= 73,  /* SPI1 Data 3 */
	CFG_SPI1_D4			= 74,  /* SPI1 Data 4 */
	CFG_SPI1_D5			= 75,  /* SPI1 Data 5 */
	CFG_SPI1_D6			= 76,  /* SPI1 Data 6 */
	CFG_SPI1_D7			= 77,  /* SPI1 Data 7 */
	CFG_SPI1_SS0		= 78,  /* SPI1 Chip Select 0 */
	CFG_SPI1_SS1		= 79,  /* SPI1 Chip Select 1 */
	CFG_SPI1_SS2		= 80,  /* SPI1 Chip Select 2 */
	CFG_SPI1_SS3		= 81,  /* SPI1 Chip Select 3 */
	CFG_SPI1_ARB		= 82,  /* SPI1 Arbitration */
	CFG_SPI1_SCLK		= 83,  /* SPI1 Serial Clock */
	CFG_SPI_SLAVE_D0	= 84,  /* SPI Slave Data 0 */
	CFG_SPI_SLAVE_SS	= 85,  /* SPI Slave Select */
	CFG_SPI_SLAVE_SCLK	= 86,  /* SPI Slave Serial Clock */
	CFG_I2S0_MCLK		= 87,  /* I2S0 Master Clock */
	CFG_I2S0_SCLK		= 88,  /* I2S0 Serial Clock(BCLK) */
	CFG_I2S0_WS			= 89,  /* I2S0 Word Select(LRCLK) */
	CFG_I2S0_IN_D0		= 90,  /* I2S0 Serial Data Input 0 */
	CFG_I2S0_IN_D1		= 91,  /* I2S0 Serial Data Input 1 */
	CFG_I2S0_IN_D2		= 92,  /* I2S0 Serial Data Input 2 */
	CFG_I2S0_IN_D3		= 93,  /* I2S0 Serial Data Input 3 */
	CFG_I2S0_OUT_D0		= 94,  /* I2S0 Serial Data Output 0 */
	CFG_I2S0_OUT_D1		= 95,  /* I2S0 Serial Data Output 1 */
	CFG_I2S0_OUT_D2		= 96,  /* I2S0 Serial Data Output 2 */
	CFG_I2S0_OUT_D3		= 97,  /* I2S0 Serial Data Output 3 */
	CFG_I2S1_MCLK		= 98,  /* I2S1 Master Clock */
	CFG_I2S1_SCLK		= 99,  /* I2S1 Serial Clock(BCLK) */
	CFG_I2S1_WS			= 100, /* I2S1 Word Select(LRCLK) */
	CFG_I2S1_IN_D0		= 101, /* I2S1 Serial Data Input 0 */
	CFG_I2S1_IN_D1		= 102, /* I2S1 Serial Data Input 1 */
	CFG_I2S1_IN_D2		= 103, /* I2S1 Serial Data Input 2 */
	CFG_I2S1_IN_D3		= 104, /* I2S1 Serial Data Input 3 */
	CFG_I2S1_OUT_D0		= 105, /* I2S1 Serial Data Output 0 */
	CFG_I2S1_OUT_D1		= 106, /* I2S1 Serial Data Output 1 */
	CFG_I2S1_OUT_D2		= 107, /* I2S1 Serial Data Output 2 */
	CFG_I2S1_OUT_D3		= 108, /* I2S1 Serial Data Output 3 */
	CFG_I2S2_MCLK		= 109, /* I2S2 Master Clock */
	CFG_I2S2_SCLK		= 110, /* I2S2 Serial Clock(BCLK) */
	CFG_I2S2_WS			= 111, /* I2S2 Word Select(LRCLK) */
	CFG_I2S2_IN_D0		= 112, /* I2S2 Serial Data Input 0 */
	CFG_I2S2_IN_D1		= 113, /* I2S2 Serial Data Input 1 */
	CFG_I2S2_IN_D2		= 114, /* I2S2 Serial Data Input 2 */
	CFG_I2S2_IN_D3		= 115, /* I2S2 Serial Data Input 3 */
	CFG_I2S2_OUT_D0		= 116, /* I2S2 Serial Data Output 0 */
	CFG_I2S2_OUT_D1		= 117, /* I2S2 Serial Data Output 1 */
	CFG_I2S2_OUT_D2		= 118, /* I2S2 Serial Data Output 2 */
	CFG_I2S2_OUT_D3		= 119, /* I2S2 Serial Data Output 3 */
	CFG_RESV0			= 120, /* Reserved function */
	CFG_RESV1			= 121, /* Reserved function */
	CFG_RESV2			= 122, /* Reserved function */
	CFG_RESV3			= 123, /* Reserved function */
	CFG_RESV4			= 124, /* Reserved function */
	CFG_RESV5			= 125, /* Reserved function */
	CFG_I2C0_SCLK		= 126, /* I2C0 Serial Clock */
	CFG_I2C0_SDA		= 127, /* I2C0 Serial Data */
	CFG_I2C1_SCLK		= 128, /* I2C1 Serial Clock */
	CFG_I2C1_SDA		= 129, /* I2C1 Serial Data */
	CFG_I2C2_SCLK		= 130, /* I2C2 Serial Clock */
	CFG_I2C2_SDA		= 131, /* I2C2 Serial Data */
	CFG_CMOS_XCLK		= 132, /* DVP System Clock */
	CFG_CMOS_RST		= 133, /* DVP System Reset */
	CFG_CMOS_PWDN		= 134, /* DVP Power Down Mode */
	CFG_CMOS_VSYNC		= 135, /* DVP Vertical Sync */
	CFG_CMOS_HREF		= 136, /* DVP Horizontal Reference output */
	CFG_CMOS_PCLK		= 137, /* Pixel Clock */
	CFG_CMOS_D0			= 138, /* Data Bit 0 */
	CFG_CMOS_D1			= 139, /* Data Bit 1 */
	CFG_CMOS_D2			= 140, /* Data Bit 2 */
	CFG_CMOS_D3			= 141, /* Data Bit 3 */
	CFG_CMOS_D4			= 142, /* Data Bit 4 */
	CFG_CMOS_D5			= 143, /* Data Bit 5 */
	CFG_CMOS_D6			= 144, /* Data Bit 6 */
	CFG_CMOS_D7			= 145, /* Data Bit 7 */
	CFG_SCCB_SCLK		= 146, /* SCCB Serial Clock */
	CFG_SCCB_SDA		= 147, /* SCCB Serial Data */
	CFG_UART1_CTS		= 148, /* UART1 Clear To Send */
	CFG_UART1_DSR		= 149, /* UART1 Data Set Ready */
	CFG_UART1_DCD		= 150, /* UART1 Data Carrier Detect */
	CFG_UART1_RI		= 151, /* UART1 Ring Indicator */
	CFG_UART1_SIR_IN	= 152, /* UART1 Serial Infrared Input */
	CFG_UART1_DTR		= 153, /* UART1 Data Terminal Ready */
	CFG_UART1_RTS		= 154, /* UART1 Request To Send */
	CFG_UART1_OUT2		= 155, /* UART1 User-designated Output 2 */
	CFG_UART1_OUT1		= 156, /* UART1 User-designated Output 1 */
	CFG_UART1_SIR_OUT	= 157, /* UART1 Serial Infrared Output */
	CFG_UART1_BAUD		= 158, /* UART1 Transmit Clock Output */
	CFG_UART1_RE		= 159, /* UART1 Receiver Output Enable */
	CFG_UART1_DE		= 160, /* UART1 Driver Output Enable */
	CFG_UART1_RS485_EN	= 161, /* UART1 RS485 Enable */
	CFG_UART2_CTS		= 162, /* UART2 Clear To Send */
	CFG_UART2_DSR		= 163, /* UART2 Data Set Ready */
	CFG_UART2_DCD		= 164, /* UART2 Data Carrier Detect */
	CFG_UART2_RI		= 165, /* UART2 Ring Indicator */
	CFG_UART2_SIR_IN	= 166, /* UART2 Serial Infrared Input */
	CFG_UART2_DTR		= 167, /* UART2 Data Terminal Ready */
	CFG_UART2_RTS		= 168, /* UART2 Request To Send */
	CFG_UART2_OUT2		= 169, /* UART2 User-designated Output 2 */
	CFG_UART2_OUT1		= 170, /* UART2 User-designated Output 1 */
	CFG_UART2_SIR_OUT	= 171, /* UART2 Serial Infrared Output */
	CFG_UART2_BAUD		= 172, /* UART2 Transmit Clock Output */
	CFG_UART2_RE		= 173, /* UART2 Receiver Output Enable */
	CFG_UART2_DE		= 174, /* UART2 Driver Output Enable */
	CFG_UART2_RS485_EN	= 175, /* UART2 RS485 Enable */
	CFG_UART3_CTS		= 176, /* UART3 Clear To Send */
	CFG_UART3_DSR		= 177, /* UART3 Data Set Ready */
	CFG_UART3_DCD		= 178, /* UART3 Data Carrier Detect */
	CFG_UART3_RI		= 179, /* UART3 Ring Indicator */
	CFG_UART3_SIR_IN	= 180, /* UART3 Serial Infrared Input */
	CFG_UART3_DTR		= 181, /* UART3 Data Terminal Ready */
	CFG_UART3_RTS		= 182, /* UART3 Request To Send */
	CFG_UART3_OUT2		= 183, /* UART3 User-designated Output 2 */
	CFG_UART3_OUT1		= 184, /* UART3 User-designated Output 1 */
	CFG_UART3_SIR_OUT	= 185, /* UART3 Serial Infrared Output */
	CFG_UART3_BAUD		= 186, /* UART3 Transmit Clock Output */
	CFG_UART3_RE		= 187, /* UART3 Receiver Output Enable */
	CFG_UART3_DE		= 188, /* UART3 Driver Output Enable */
	CFG_UART3_RS485_EN	= 189, /* UART3 RS485 Enable */
	CFG_TIMER0_TOGGLE1	= 190, /* TIMER0 Toggle Output 1 */
	CFG_TIMER0_TOGGLE2	= 191, /* TIMER0 Toggle Output 2 */
	CFG_TIMER0_TOGGLE3	= 192, /* TIMER0 Toggle Output 3 */
	CFG_TIMER0_TOGGLE4	= 193, /* TIMER0 Toggle Output 4 */
	CFG_TIMER1_TOGGLE1	= 194, /* TIMER1 Toggle Output 1 */
	CFG_TIMER1_TOGGLE2	= 195, /* TIMER1 Toggle Output 2 */
	CFG_TIMER1_TOGGLE3	= 196, /* TIMER1 Toggle Output 3 */
	CFG_TIMER1_TOGGLE4	= 197, /* TIMER1 Toggle Output 4 */
	CFG_TIMER2_TOGGLE1	= 198, /* TIMER2 Toggle Output 1 */
	CFG_TIMER2_TOGGLE2	= 199, /* TIMER2 Toggle Output 2 */
	CFG_TIMER2_TOGGLE3	= 200, /* TIMER2 Toggle Output 3 */
	CFG_TIMER2_TOGGLE4	= 201, /* TIMER2 Toggle Output 4 */
	CFG_CLK_SPI2		= 202, /* Clock SPI2 */
	CFG_CLK_I2C2		= 203, /* Clock I2C2 */
	CFG_INTERNAL0		= 204, /* Internal function signal 0 */
	CFG_INTERNAL1		= 205, /* Internal function signal 1 */
	CFG_INTERNAL2		= 206, /* Internal function signal 2 */
	CFG_INTERNAL3		= 207, /* Internal function signal 3 */
	CFG_INTERNAL4		= 208, /* Internal function signal 4 */
	CFG_INTERNAL5		= 209, /* Internal function signal 5 */
	CFG_INTERNAL6		= 210, /* Internal function signal 6 */
	CFG_INTERNAL7		= 211, /* Internal function signal 7 */
	CFG_INTERNAL8		= 212, /* Internal function signal 8 */
	CFG_INTERNAL9		= 213, /* Internal function signal 9 */
	CFG_INTERNAL10		= 214, /* Internal function signal 10 */
	CFG_INTERNAL11		= 215, /* Internal function signal 11 */
	CFG_INTERNAL12		= 216, /* Internal function signal 12 */
	CFG_INTERNAL13		= 217, /* Internal function signal 13 */
	CFG_INTERNAL14		= 218, /* Internal function signal 14 */
	CFG_INTERNAL15		= 219, /* Internal function signal 15 */
	CFG_INTERNAL16		= 220, /* Internal function signal 16 */
	CFG_INTERNAL17		= 221, /* Internal function signal 17 */
	CFG_CONSTANT		= 222, /* Constant function */
	CFG_INTERNAL18		= 223, /* Internal function signal 18 */
	CFG_DEBUG0			= 224, /* Debug function 0 */
	CFG_DEBUG1			= 225, /* Debug function 1 */
	CFG_DEBUG2			= 226, /* Debug function 2 */
	CFG_DEBUG3			= 227, /* Debug function 3 */
	CFG_DEBUG4			= 228, /* Debug function 4 */
	CFG_DEBUG5			= 229, /* Debug function 5 */
	CFG_DEBUG6			= 230, /* Debug function 6 */
	CFG_DEBUG7			= 231, /* Debug function 7 */
	CFG_DEBUG8		 	= 232, /* Debug function 8 */
	CFG_DEBUG9		 	= 233, /* Debug function 9 */
	CFG_DEBUG10			= 234, /* Debug function 10 */
	CFG_DEBUG11			= 235, /* Debug function 11 */
	CFG_DEBUG12			= 236, /* Debug function 12 */
	CFG_DEBUG13			= 237, /* Debug function 13 */
	CFG_DEBUG14			= 238, /* Debug function 14 */
	CFG_DEBUG15			= 239, /* Debug function 15 */
	CFG_DEBUG16			= 240, /* Debug function 16 */
	CFG_DEBUG17			= 241, /* Debug function 17 */
	CFG_DEBUG18			= 242, /* Debug function 18 */
	CFG_DEBUG19			= 243, /* Debug function 19 */
	CFG_DEBUG20			= 244, /* Debug function 20 */
	CFG_DEBUG21			= 245, /* Debug function 21 */
	CFG_DEBUG22			= 246, /* Debug function 22 */
	CFG_DEBUG23			= 247, /* Debug function 23 */
	CFG_DEBUG24			= 248, /* Debug function 24 */
	CFG_DEBUG25			= 249, /* Debug function 25 */
	CFG_DEBUG26			= 250, /* Debug function 26 */
	CFG_DEBUG27			= 251, /* Debug function 27 */
	CFG_DEBUG28			= 252, /* Debug function 28 */
	CFG_DEBUG29			= 253, /* Debug function 29 */
	CFG_DEBUG30			= 254, /* Debug function 30 */
	CFG_DEBUG31			= 255, /* Debug function 31 */
};

#define K210_GPIO0		(0)
#define K210_GPIO1		(1)
#define K210_GPIO2		(2)
#define K210_GPIO3		(3)
#define K210_GPIO4		(4)
#define K210_GPIO5		(5)
#define K210_GPIO6		(6)
#define K210_GPIO7		(7)
#define K210_GPIO8		(8)
#define K210_GPIO9		(9)
#define K210_GPIO10		(10)
#define K210_GPIO11		(11)
#define K210_GPIO12		(12)
#define K210_GPIO13		(13)
#define K210_GPIO14		(14)
#define K210_GPIO15		(15)
#define K210_GPIO16		(16)
#define K210_GPIO17		(17)
#define K210_GPIO18		(18)
#define K210_GPIO19		(19)
#define K210_GPIO20		(20)
#define K210_GPIO21		(21)
#define K210_GPIO22		(22)
#define K210_GPIO23		(23)
#define K210_GPIO24		(24)
#define K210_GPIO25		(25)
#define K210_GPIO26		(26)
#define K210_GPIO27		(27)
#define K210_GPIO28		(28)
#define K210_GPIO29		(29)
#define K210_GPIO30		(30)
#define K210_GPIO31		(31)
#define K210_GPIO32		(32)
#define K210_GPIO33		(33)
#define K210_GPIO34		(34)
#define K210_GPIO35		(35)
#define K210_GPIO36		(36)
#define K210_GPIO37		(37)
#define K210_GPIO38		(38)
#define K210_GPIO39		(39)
#define K210_GPIO40		(40)
#define K210_GPIO41		(41)
#define K210_GPIO42		(42)
#define K210_GPIO43		(43)
#define K210_GPIO44		(44)
#define K210_GPIO45		(45)
#define K210_GPIO46		(46)
#define K210_GPIO47		(47)

#ifdef __cplusplus
}
#endif

#endif /* __K210_GPIO_H__ */
