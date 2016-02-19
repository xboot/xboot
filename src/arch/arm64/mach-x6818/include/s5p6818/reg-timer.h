#ifndef __S5P6818_REG_TIMER_H__
#define __S5P6818_REG_TIMER_H__

#define S5P6818_TIMER_BASE	(0xC0017000)

#define TIMER_TCFG0			(0x00)
#define TIMER_TCFG1			(0x04)
#define TIMER_TCON			(0x08)
#define TIMER_TSTAT			(0x44)
#define TIMER_TCNTB(ch)		((ch + 1) * 0xC + 0x00)
#define TIMER_TCMPB(ch)		((ch + 1) * 0xC + 0x04)
#define TIMER_TCNTO(ch)		((ch + 1) * 0xC + 0x08)

#endif /* __S5P6818_REG_TIMER_H__ */
