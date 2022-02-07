/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_GPIO_H__
#define __GX6605S_GPIO_H__

#include <gx6605s.h>

enum gx6605s_gpio_registers {
    GX6605S_GPIO_DIROUT = 0x00, /* Direction (0: input, 1: output) */
    GX6605S_GPIO_DAT    = 0x04, /* Level (0: low, 1: high) */
    GX6605S_GPIO_SET    = 0x08, /* High Set (only for output mode) */
    GX6605S_GPIO_CLR    = 0x0c, /* Low Set (only for output mode) */
	GX6605S_GPIO_INTC   = 0x10, /* Interrupt Gate (may not support) */
	GX6605S_GPIO_INTS   = 0x14, /* Interrupt Status (may not support) */
	GX6605S_GPIO_TIG    = 0x18, /* Interrupt Trigger Mode (may not support) */
	GX6605S_GPIO_INV    = 0x1c, /* Inverse Mode For Input/Interrupt (may not support) */
	GX6605S_GPIO_ODR    = 0x20,
};

#endif  /* __GX6605S_GPIO_H__ */
