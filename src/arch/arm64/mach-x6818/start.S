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

	mov x0, #0x62
	bl debug

	bl copy_shadow

	mov x0, #0x62
	bl debug
	mov x0, #0x64
	bl debug

	bl clear_bss

	mov x0, #0x63
	bl debug

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
