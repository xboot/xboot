/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_GPIO_H__
#define __GX6605S_GPIO_H__

#include <gx6605s.h>

enum gx6605s_gpio_registers {
    GX6605S_GPIO_DIROUT = 0x00,
    GX6605S_GPIO_DAT    = 0x04,
    GX6605S_GPIO_SET    = 0x08,
    GX6605S_GPIO_CLR    = 0x0c,
};

#endif  /* __GX6605S_GPIO_H__ */
