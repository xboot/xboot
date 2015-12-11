#ifndef __S5P6818_REG_GIC_H__
#define __S5P6818_REG_GIC_H__

#define S5P6818_GIC_CPU_BASE	(0xc000a000)
#define S5P6818_GIC_DIST_BASE	(0xc0009000)

#define CPU_CTRL				(0x00)
#define CPU_PRIMASK				(0x04)
#define CPU_BINPOINT			(0x08)
#define CPU_INTACK				(0x0c)
#define CPU_EOI					(0x10)
#define CPU_RUNNINGPRI			(0x14)
#define CPU_HIGHPRI				(0x18)

#define DIST_CTRL				(0x000)
#define DIST_CTR				(0x004)
#define DIST_ENABLE_SET			(0x100)
#define DIST_ENABLE_CLEAR		(0x180)
#define DIST_PENDING_SET		(0x200)
#define DIST_PENDING_CLEAR		(0x280)
#define DIST_ACTIVE_BIT			(0x300)
#define DIST_PRI				(0x400)
#define DIST_TARGET				(0x800)
#define DIST_CONFIG				(0xc00)
#define DIST_SOFTINT			(0xf00)

#endif /* __S5P6818_REG_GIC_H__ */
