#ifndef __REALVIEW_REG_GPIO_H__
#define __REALVIEW_REG_GPIO_H__

#define REALVIEW_GPIO0_BASE		(0x10013000)
#define REALVIEW_GPIO1_BASE		(0x10014000)
#define REALVIEW_GPIO2_BASE		(0x10015000)

#define GPIO_DIR				(0x400)
#define GPIO_IS					(0x404)
#define GPIO_IBE				(0x408)
#define GPIO_IEV				(0x40C)
#define GPIO_IE					(0x410)
#define GPIO_RIS				(0x414)
#define GPIO_MIS				(0x418)
#define GPIO_IC					(0x41C)

#endif /* __REALVIEW_REG_GPIO_H__ */
