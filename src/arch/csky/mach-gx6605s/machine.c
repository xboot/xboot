/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <xboot.h>
#include <gx6605s.h>

virtual_addr_t phys_to_virt(physical_addr_t phys)
{
    return phys + SSEG1_BASE;
}

physical_addr_t virt_to_phys(virtual_addr_t virt)
{
    return virt - SSEG1_BASE;
}
