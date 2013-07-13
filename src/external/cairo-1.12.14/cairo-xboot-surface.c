/* cairo - a vector graphics library with display and print output
 *
 * Copyright © 2009 Chris Wilson
 *
 * This library is free software; you can redistribute it and/or
 * modify it either under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * (the "LGPL") or, at your option, under the terms of the Mozilla
 * Public License Version 1.1 (the "MPL"). If you do not alter this
 * notice, a recipient may use your version of this file under either
 * the MPL or the LGPL.
 *
 * You should have received a copy of the LGPL along with this library
 * in the file COPYING-LGPL-2.1; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA
 * You should have received a copy of the MPL along with this library
 * in the file COPYING-MPL-1.1
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY
 * OF ANY KIND, either express or implied. See the LGPL or the MPL for
 * the specific language governing rights and limitations.
 *
 * The Original Code is the cairo graphics library.
 *
 * The Initial Developer of the Original Code is Chris Wilson.
 *
 * Contributor(s):
 *      Chris Wilson <chris@chris-wilson.co.uk>
 */


#include "cairoint.h"

#include "cairo-xboot.h"

#include "cairo-clip-private.h"
#include "cairo-device-private.h"
#include "cairo-default-context-private.h"
#include "cairo-image-surface-private.h"
#include "cairo-error-private.h"
#include "cairo-output-stream-private.h"
#include "cairo-recording-surface-inline.h"
#include "cairo-surface-wrapper-private.h"
#include "cairo-array-private.h"
#include "cairo-image-surface-inline.h"

struct cairo_xboot_surface_t {
	void * pixels;
	int width;
	int height;
	cairo_format_t fmt;

	struct surface_t * surface;
	cairo_surface_t * cs;
};

static cairo_format_t cairo_format_from_pixel_format (enum pixel_format fmt)
{
	switch(fmt)
	{
	case PIXEL_FORMAT_GENERIC:
		return CAIRO_FORMAT_INVALID;

	case PIXEL_FORMAT_ABGR_8888:
		return CAIRO_FORMAT_INVALID;
	case PIXEL_FORMAT_ARGB_8888:
		return CAIRO_FORMAT_ARGB32;
	case PIXEL_FORMAT_BGRA_8888:
		return CAIRO_FORMAT_INVALID;
	case PIXEL_FORMAT_RGBA_8888:
		return CAIRO_FORMAT_INVALID;

	case PIXEL_FORMAT_BGR_888:
		return CAIRO_FORMAT_INVALID;
	case PIXEL_FORMAT_RGB_888:
		return CAIRO_FORMAT_RGB24;

	case PIXEL_FORMAT_BGR_565:
		return CAIRO_FORMAT_INVALID;
	case PIXEL_FORMAT_RGB_565:
		return CAIRO_FORMAT_RGB16_565;

	case PIXEL_FORMAT_BGR_332:
		return CAIRO_FORMAT_INVALID;
	case PIXEL_FORMAT_RGB_332:
		return CAIRO_FORMAT_INVALID;

	default:
		break;
	}

	return CAIRO_FORMAT_INVALID;
}

static void cairo_xboot_surface_destroy (void * data)
{
	struct cairo_xboot_surface_t * xs = (struct cairo_xboot_surface_t *) data;

	if (!xs)
		return;

	free(xs);
}

cairo_surface_t *
cairo_xboot_surface_create (struct surface_t * surface)
{
	struct cairo_xboot_surface_t * xs;

	if (!surface)
		return _cairo_surface_create_in_error(_cairo_error(CAIRO_STATUS_NULL_POINTER));

	xs = malloc(sizeof(struct cairo_xboot_surface_t));
	if (!xs)
		return _cairo_surface_create_in_error(_cairo_error(CAIRO_STATUS_NO_MEMORY));

	xs->pixels = surface->pixels;
	xs->width = surface->w;
	xs->height = surface->h;
	xs->fmt = cairo_format_from_pixel_format(surface->info.fmt);

	xs->surface = surface;
	xs->cs = cairo_image_surface_create_for_data(xs->pixels, xs->fmt, xs->width,
			xs->height, cairo_format_stride_for_width(xs->fmt, surface->w));
	cairo_surface_set_user_data(xs->cs, NULL, xs, &cairo_xboot_surface_destroy);

	return xs->cs;
}
