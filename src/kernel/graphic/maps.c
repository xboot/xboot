/*
 * kernel/graphic/maps.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
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

#include <graphic/maps.h>
#include <graphic/maps/software.h>

bool_t surface_set_maps(struct surface_maps * maps)
{
	if(!maps)
		return FALSE;

	if(!maps->point)
		maps->point = map_software_point;

	if(!maps->hline)
		maps->hline = map_software_hline;

	if(!maps->vline)
		maps->vline = map_software_vline;

	if(!maps->fill)
		maps->fill = map_software_fill;

	if(!maps->blit)
		maps->blit = map_software_blit;

	if(!maps->scale)
		maps->scale = map_software_scale;

	if(!maps->rotate)
		maps->rotate = map_software_rotate;

	if(!maps->transform)
		maps->transform = map_software_transform;

	return TRUE;
}
