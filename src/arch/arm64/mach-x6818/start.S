/*
 * start.S
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

	.global	_start
_start:
	mov x0, #0x61
	bl debug

	/* Copy shadow of data section */
copy_shadow_data:
	ldr	x0, _text_start
	ldr x1, _text_end
	sub x0, x1, x0
	adr	x1, _start
	add x0, x0 ,x1
	ldr	x1, _data_start
	ldr	x2, _data_shadow_start
	ldr	x3, _data_shadow_end
	sub	x2, x3, x2
	add	x2, x0, x2
	bl	memcpy

	/* Clear bss section */
clear_bss:
	ldr	x0, _bss_start
	ldr	x1, _bss_end
	sub x1, x1, x0
	mov x2, #0
	bl	memset

	bl	xboot_main

/*
 * The section location
 */
 	.align 8
_text_start:
	.long	__text_start
_text_end:
	.long	__text_end
_romdisk_start:
	.long	__romdisk_start
_romdisk_end:
	.long	__romdisk_end
_data_shadow_start:
	.long	 __data_shadow_start
_data_shadow_end:
	.long	 __data_shadow_end
_data_start:
	.long	__data_start
_data_end:
	.long	__data_end
_bss_start:
	.long	__bss_start
_bss_end:
	.long	__bss_end

.end
