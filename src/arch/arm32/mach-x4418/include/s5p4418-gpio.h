#ifndef __S5P4418_GPIO_H__
#define __S5P4418_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <sizes.h>
#include <stddef.h>

#define S5P4418_GPIOA(x)			(x)
#define S5P4418_GPIOB(x)			(x + 32)
#define S5P4418_GPIOC(x)			(x + 64)
#define S5P4418_GPIOD(x)			(x + 96)
#define S5P4418_GPIOE(x)			(x + 128)
#define S5P4418_GPIOALV(x)			(x + 160)

static const char * __gpio_irq_array[] = {
	"GPIOA0",
	"GPIOA1",
	"GPIOA2",
	"GPIOA3",
	"GPIOA4",
	"GPIOA5",
	"GPIOA6",
	"GPIOA7",
	"GPIOA8",
	"GPIOA9",
	"GPIOA10",
	"GPIOA11",
	"GPIOA12",
	"GPIOA13",
	"GPIOA14",
	"GPIOA15",
	"GPIOA16",
	"GPIOA17",
	"GPIOA18",
	"GPIOA19",
	"GPIOA20",
	"GPIOA21",
	"GPIOA22",
	"GPIOA23",
	"GPIOA24",
	"GPIOA25",
	"GPIOA26",
	"GPIOA27",
	"GPIOA28",
	"GPIOA29",
	"GPIOA30",
	"GPIOA31",

	"GPIOB0",
	"GPIOB1",
	"GPIOB2",
	"GPIOB3",
	"GPIOB4",
	"GPIOB5",
	"GPIOB6",
	"GPIOB7",
	"GPIOB8",
	"GPIOB9",
	"GPIOB10",
	"GPIOB11",
	"GPIOB12",
	"GPIOB13",
	"GPIOB14",
	"GPIOB15",
	"GPIOB16",
	"GPIOB17",
	"GPIOB18",
	"GPIOB19",
	"GPIOB20",
	"GPIOB21",
	"GPIOB22",
	"GPIOB23",
	"GPIOB24",
	"GPIOB25",
	"GPIOB26",
	"GPIOB27",
	"GPIOB28",
	"GPIOB29",
	"GPIOB30",
	"GPIOB31",

	"GPIOC0",
	"GPIOC1",
	"GPIOC2",
	"GPIOC3",
	"GPIOC4",
	"GPIOC5",
	"GPIOC6",
	"GPIOC7",
	"GPIOC8",
	"GPIOC9",
	"GPIOC10",
	"GPIOC11",
	"GPIOC12",
	"GPIOC13",
	"GPIOC14",
	"GPIOC15",
	"GPIOC16",
	"GPIOC17",
	"GPIOC18",
	"GPIOC19",
	"GPIOC20",
	"GPIOC21",
	"GPIOC22",
	"GPIOC23",
	"GPIOC24",
	"GPIOC25",
	"GPIOC26",
	"GPIOC27",
	"GPIOC28",
	"GPIOC29",
	"GPIOC30",
	"GPIOC31",

	"GPIOD0",
	"GPIOD1",
	"GPIOD2",
	"GPIOD3",
	"GPIOD4",
	"GPIOD5",
	"GPIOD6",
	"GPIOD7",
	"GPIOD8",
	"GPIOD9",
	"GPIOD10",
	"GPIOD11",
	"GPIOD12",
	"GPIOD13",
	"GPIOD14",
	"GPIOD15",
	"GPIOD16",
	"GPIOD17",
	"GPIOD18",
	"GPIOD19",
	"GPIOD20",
	"GPIOD21",
	"GPIOD22",
	"GPIOD23",
	"GPIOD24",
	"GPIOD25",
	"GPIOD26",
	"GPIOD27",
	"GPIOD28",
	"GPIOD29",
	"GPIOD30",
	"GPIOD31",

	"GPIOE0",
	"GPIOE1",
	"GPIOE2",
	"GPIOE3",
	"GPIOE4",
	"GPIOE5",
	"GPIOE6",
	"GPIOE7",
	"GPIOE8",
	"GPIOE9",
	"GPIOE10",
	"GPIOE11",
	"GPIOE12",
	"GPIOE13",
	"GPIOE14",
	"GPIOE15",
	"GPIOE16",
	"GPIOE17",
	"GPIOE18",
	"GPIOE19",
	"GPIOE20",
	"GPIOE21",
	"GPIOE22",
	"GPIOE23",
	"GPIOE24",
	"GPIOE25",
	"GPIOE26",
	"GPIOE27",
	"GPIOE28",
	"GPIOE29",
	"GPIOE30",
	"GPIOE31",

	"GPIOALV0",
	"GPIOALV1",
	"GPIOALV2",
	"GPIOALV3",
	"GPIOALV4",
	"GPIOALV5",
};

static inline const char * gpio_to_irq(int gpio)
{
	if(gpio < S5P4418_GPIOA(0) || gpio > S5P4418_GPIOALV(5))
		return NULL;
	return __gpio_irq_array[gpio];
}

static inline int irq_to_gpio(const char * irq)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(__gpio_irq_array); i++)
	{
		if(strcmp(__gpio_irq_array[i], irq) == 0)
			return i;
	}
	return -1;
}

#ifdef __cplusplus
}
#endif

#endif /* __S5P4418_GPIO_H__ */
