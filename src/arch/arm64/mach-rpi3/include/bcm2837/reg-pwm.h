#ifndef __BCM2837_REG_PWM_H__
#define __BCM2837_REG_PWM_H__

#define BCM2837_PWM_BASE	(0x3f20c000)

#define PWM_CTRL			(0x00)
#define PWM_CTRL_MASK		(0xff)
#define PWM_CTRL_SHIFT(x)	((x) * 8)
#define PWM_STA				(0x04)
#define PWM_DMAC			(0x08)
#define PWM_FIFO			(0x18)
#define PWM_PERIOD(x)		(((x) * 0x10) + 0x10)
#define PWM_DUTY(x)			(((x) * 0x10) + 0x14)

#endif /* __BCM2837_REG_PWM_H__ */
