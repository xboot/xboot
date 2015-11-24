#ifndef __REALVIEW_REG_TIMER_H__
#define __REALVIEW_REG_TIMER_H__

#define REALVIEW_TIMER0_BASE	(0x10011000)
#define REALVIEW_TIMER1_BASE	(0x10011020)
#define REALVIEW_TIMER2_BASE	(0x10012000)
#define REALVIEW_TIMER3_BASE	(0x10012020)

#define TIMER_LOAD				(0x00)
#define TIMER_VALUE				(0x04)
#define TIMER_CTRL				(0x08)
#define TIMER_ICLR				(0x0c)
#define TIMER_RIS				(0x10)
#define TIMER_MIS				(0x14)
#define TIMER_BGLOAD			(0x18)

#define TC_ONESHOT				(1 << 0)
#define TC_32BIT				(1 << 1)
#define TC_DIV1					(0 << 2)
#define TC_DIV16				(1 << 2)
#define TC_DIV256				(2 << 2)
#define TC_IE					(1 << 5)
#define TC_PERIODIC				(1 << 6)
#define TC_ENABLE				(1 << 7)

#endif /* __REALVIEW_REG_TIMER_H__ */
