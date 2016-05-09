/*
 * driver/input/rc/rc-core.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <input/rc/rc-core.h>

uint32_t rc_decoder_handle(struct rc_decoder_t * decoder, int pulse, int duration)
{
	uint32_t code;
	int i;

	if((code = rc_nec_decoder_handle(&decoder->nec, pulse, duration)) != 0)
	{
	}

	for(i = 0; i < decoder->size; i++)
	{
		if(decoder->map[i].scan == code)
			return decoder->map[i].key;
	}
	return code;
}
