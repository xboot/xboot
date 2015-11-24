#ifndef __REALVIEW_REG_MOUSE_H__
#define __REALVIEW_REG_MOUSE_H__

#define REALVIEW_MOUSE_BASE	 (0x10007000)

#define MOUSE_CR	 		(0x00)
#define MOUSE_STAT	 		(0x04)
#define MOUSE_DATA	 		(0x08)
#define MOUSE_CLKDIV 		(0x0c)
#define MOUSE_IIR	 		(0x10)

#define MOUSE_CR_TYPE	 	(1 << 5)
#define MOUSE_CR_RXINTREN	(1 << 4)
#define MOUSE_CR_TXINTREN	(1 << 3)
#define MOUSE_CR_EN			(1 << 2)
#define MOUSE_CR_FD			(1 << 1)
#define MOUSE_CR_FC			(1 << 0)

#define MOUSE_STAT_TXEMPTY	(1 << 6)
#define MOUSE_STAT_TXBUSY	(1 << 5)
#define MOUSE_STAT_RXFULL	(1 << 4)
#define MOUSE_STAT_RXBUSY	(1 << 3)
#define MOUSE_STAT_RXPARITY	(1 << 2)
#define MOUSE_STAT_IC		(1 << 1)
#define MOUSE_STAT_ID		(1 << 0)

#define MOUSE_IIR_TXINTR	(1 << 1)
#define MOUSE_IIR_RXINTR	(1 << 0)

#endif /* __REALVIEW_REG_MOUSE_H__ */
