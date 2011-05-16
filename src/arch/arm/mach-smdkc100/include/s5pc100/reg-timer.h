#ifndef __S5PC100_REG_TIMER_H__
#define __S5PC100_REG_TIMER_H__

#include <xboot.h>

/*
 * timer 0 ~ 4
 */
#define S5PC100_TCFG0	  			(0xea000000)
#define S5PC100_TCFG1	  			(0xea000004)
#define S5PC100_TCON	  			(0xea000008)

#define S5PC100_TCNTB0	  			(0xea00000c)
#define S5PC100_TCMPB0	  			(0xea000010)
#define S5PC100_TCNTO0	  			(0xea000014)

#define S5PC100_TCNTB1	  			(0xea000018)
#define S5PC100_TCMPB1	  			(0xea00001c)
#define S5PC100_TCNTO1	  			(0xea000020)

#define S5PC100_TCNTB2	  			(0xea000024)
#define S5PC100_TCNTO2	  			(0xea000028)

#define S5PC100_TCNTB3	  			(0xea00002c)
#define S5PC100_TCNTO3	  			(0xea000030)

#define S5PC100_TCNTB4	  			(0xea00003c)
#define S5PC100_TCNTO4	  			(0xea000040)

#define S5PC100_TINT_CSTAT	  		(0xea000044)

/*
 * system timer
 */
#define S5PC100_SYSTIMER_TCFG		(0xea100000)
#define S5PC100_SYSTIMER_TCON		(0xea100004)

#define S5PC100_SYSTIMER_TCNTB		(0xea100008)
#define S5PC100_SYSTIMER_TCNTO		(0xea10000c)

#define S5PC100_SYSTIMER_ICNTB		(0xea100010)
#define S5PC100_SYSTIMER_ICNTO		(0xea100014)

#define S5PC100_SYSTIMER_TINT_CSTAT	(0xea100018)


#endif /* __S5PC100_REG_TIMER_H__ */
