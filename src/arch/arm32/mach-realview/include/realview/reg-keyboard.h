#ifndef __REALVIEW_REG_KEYBOARD_H__
#define __REALVIEW_REG_KEYBOARD_H__

#define REALVIEW_KEYBOARD_BASE	(0x10006000)

#define KEYBOARD_CR	 			(0x00)
#define KEYBOARD_STAT	 		(0x04)
#define KEYBOARD_DATA	 		(0x08)
#define KEYBOARD_CLKDIV 		(0x0c)
#define KEYBOARD_IIR	 		(0x10)

#define KEYBOARD_CR_TYPE	 	(1 << 5)
#define KEYBOARD_CR_RXINTREN	(1 << 4)
#define KEYBOARD_CR_TXINTREN	(1 << 3)
#define KEYBOARD_CR_EN			(1 << 2)
#define KEYBOARD_CR_FD			(1 << 1)
#define KEYBOARD_CR_FC			(1 << 0)

#define KEYBOARD_STAT_TXEMPTY	(1 << 6)
#define KEYBOARD_STAT_TXBUSY	(1 << 5)
#define KEYBOARD_STAT_RXFULL	(1 << 4)
#define KEYBOARD_STAT_RXBUSY	(1 << 3)
#define KEYBOARD_STAT_RXPARITY	(1 << 2)
#define KEYBOARD_STAT_IC		(1 << 1)
#define KEYBOARD_STAT_ID		(1 << 0)

#define KEYBOARD_IIR_TXINTR		(1 << 1)
#define KEYBOARD_IIR_RXINTR		(1 << 0)

#endif /* __REALVIEW_REG_KEYBOARD_H__ */
