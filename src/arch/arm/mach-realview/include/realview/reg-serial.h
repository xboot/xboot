#ifndef __REALVIEW_REG_SERIAL_H__
#define __REALVIEW_REG_SERIAL_H__

/*
 * realview serial 0
 */
#define REALVIEW_SERIAL0_DATA	 		(0x10009000 + 0x00)		/* data read or written from the interface */
#define REALVIEW_SERIAL0_RSR 	 		(0x10009000 + 0x04)		/* receive status register */
#define REALVIEW_SERIAL0_FR 	 		(0x10009000 + 0x18)		/* flag register (read only) */

#define REALVIEW_SERIAL0_IBRD 	 		(0x10009000 + 0x24)
#define REALVIEW_SERIAL0_FBRD 	 		(0x10009000 + 0x28)
#define REALVIEW_SERIAL0_LCRH 	 		(0x10009000 + 0x2c)
#define REALVIEW_SERIAL0_CR 	 		(0x10009000 + 0x30)
#define REALVIEW_SERIAL0_IMSC 	 		(0x10009000 + 0x38)

/*
 * realview serial 1
 */
#define REALVIEW_SERIAL1_DATA	 		(0x1000a000 + 0x00)		/* data read or written from the interface */
#define REALVIEW_SERIAL1_RSR 	 		(0x1000a000 + 0x04)		/* receive status register */
#define REALVIEW_SERIAL1_FR 	 		(0x1000a000 + 0x18)		/* flag register (read only) */

#define REALVIEW_SERIAL1_IBRD 	 		(0x1000a000 + 0x24)
#define REALVIEW_SERIAL1_FBRD 	 		(0x1000a000 + 0x28)
#define REALVIEW_SERIAL1_LCRH 	 		(0x1000a000 + 0x2c)
#define REALVIEW_SERIAL1_CR 	 		(0x1000a000 + 0x30)
#define REALVIEW_SERIAL1_IMSC 	 		(0x1000a000 + 0x38)

/*
 * realview serial 2
 */
#define REALVIEW_SERIAL2_DATA	 		(0x1000b000 + 0x00)		/* data read or written from the interface */
#define REALVIEW_SERIAL2_RSR 	 		(0x1000b000 + 0x04)		/* receive status register */
#define REALVIEW_SERIAL2_FR 	 		(0x1000b000 + 0x18)		/* flag register (read only) */

#define REALVIEW_SERIAL2_IBRD 	 		(0x1000b000 + 0x24)
#define REALVIEW_SERIAL2_FBRD 	 		(0x1000b000 + 0x28)
#define REALVIEW_SERIAL2_LCRH 	 		(0x1000b000 + 0x2c)
#define REALVIEW_SERIAL2_CR 	 		(0x1000b000 + 0x30)
#define REALVIEW_SERIAL2_IMSC 	 		(0x1000b000 + 0x38)

/*
 * realview serial 3
 */
#define REALVIEW_SERIAL3_DATA	 		(0x1000c000 + 0x00)		/* data read or written from the interface */
#define REALVIEW_SERIAL3_RSR 	 		(0x1000c000 + 0x04)		/* receive status register */
#define REALVIEW_SERIAL3_FR 	 		(0x1000c000 + 0x18)		/* flag register (read only) */

#define REALVIEW_SERIAL3_IBRD 	 		(0x1000c000 + 0x24)
#define REALVIEW_SERIAL3_FBRD 	 		(0x1000c000 + 0x28)
#define REALVIEW_SERIAL3_LCRH 	 		(0x1000c000 + 0x2c)
#define REALVIEW_SERIAL3_CR 	 		(0x1000c000 + 0x30)
#define REALVIEW_SERIAL3_IMSC 	 		(0x1000c000 + 0x38)


#define REALVIEW_SERIAL_RSR_OE			(0x08)
#define REALVIEW_SERIAL_RSR_BE			(0x04)
#define REALVIEW_SERIAL_RSR_PE			(0x02)
#define REALVIEW_SERIAL_RSR_FE			(0x01)

#define REALVIEW_SERIAL_FR_TXFE			(0x80)
#define REALVIEW_SERIAL_FR_RXFF			(0x40)
#define REALVIEW_SERIAL_FR_TXFF			(0x20)
#define REALVIEW_SERIAL_FR_RXFE			(0x10)
#define REALVIEW_SERIAL_FR_BUSY			(0x08)

#define REALVIEW_SERIAL_LCRH_SPS		(1 << 7)
#define REALVIEW_SERIAL_LCRH_WLEN_8		(3 << 5)
#define REALVIEW_SERIAL_LCRH_WLEN_7		(2 << 5)
#define REALVIEW_SERIAL_LCRH_WLEN_6		(1 << 5)
#define REALVIEW_SERIAL_LCRH_WLEN_5		(0 << 5)
#define REALVIEW_SERIAL_LCRH_FEN		(1 << 4)
#define REALVIEW_SERIAL_LCRH_STP2		(1 << 3)
#define REALVIEW_SERIAL_LCRH_EPS		(1 << 2)
#define REALVIEW_SERIAL_LCRH_PEN		(1 << 1)
#define REALVIEW_SERIAL_LCRH_BRK		(1 << 0)

#define REALVIEW_SERIAL_CR_CTSEN		(1 << 15)
#define REALVIEW_SERIAL_CR_RTSEN		(1 << 14)
#define REALVIEW_SERIAL_CR_OUT2			(1 << 13)
#define REALVIEW_SERIAL_CR_OUT1			(1 << 12)
#define REALVIEW_SERIAL_CR_RTS			(1 << 11)
#define REALVIEW_SERIAL_CR_DTR			(1 << 10)
#define REALVIEW_SERIAL_CR_RXE			(1 << 9)
#define REALVIEW_SERIAL_CR_TXE			(1 << 8)
#define REALVIEW_SERIAL_CR_LPE			(1 << 7)
#define REALVIEW_SERIAL_CR_IIRLP		(1 << 2)
#define REALVIEW_SERIAL_CR_SIREN		(1 << 1)
#define REALVIEW_SERIAL_CR_UARTEN		(1 << 0)

#define REALVIEW_SERIAL_IMSC_OEIM		(1 << 10)
#define REALVIEW_SERIAL_IMSC_BEIM		(1 << 9)
#define REALVIEW_SERIAL_IMSC_PEIM		(1 << 8)
#define REALVIEW_SERIAL_IMSC_FEIM		(1 << 7)
#define REALVIEW_SERIAL_IMSC_RTIM		(1 << 6)
#define REALVIEW_SERIAL_IMSC_TXIM		(1 << 5)
#define REALVIEW_SERIAL_IMSC_RXIM		(1 << 4)
#define REALVIEW_SERIAL_IMSC_DSRMIM		(1 << 3)
#define REALVIEW_SERIAL_IMSC_DCDMIM		(1 << 2)
#define REALVIEW_SERIAL_IMSC_CTSMIM		(1 << 1)
#define REALVIEW_SERIAL_IMSC_RIMIM		(1 << 0)


#endif /* __REALVIEW_REG_SERIAL_H__ */
