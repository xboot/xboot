#ifndef __S5P6818_REG_PWM_H__
#define __S5P6818_REG_PWM_H__

#define S5P6818_PWM_BASE	(0xC0018000)

#define PWM_TCFG0			(0x00)
#define PWM_TCFG1			(0x04)
#define PWM_TCON			(0x08)
#define PWM_TSTAT			(0x44)
#define PWM_TCNTB(ch)		((ch + 1) * 0xC + 0x00)
#define PWM_TCMPB(ch)		((ch + 1) * 0xC + 0x04)
#define PWM_TCNTO(ch)		((ch + 1) * 0xC + 0x08)

#endif /* __S5P6818_REG_PWM_H__ */
