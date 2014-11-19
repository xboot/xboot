/*
 * init/version.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
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

#include <version.h>

#define VERSION_TO_STRING(major, minor, patch) \
	#major"."#minor"."#patch

#define XBOOT_VERSION(major, minor, patch) \
	((major * 10000) + (minor * 100) + (patch * 1))

#define XBOOT_VERSION_STRING(major, minor, patch) \
	VERSION_TO_STRING(major, minor, patch)

#define XBOOT_BANNER_STRING(major, minor, patch) \
	"xboot version: " \
	VERSION_TO_STRING(major, minor, patch) \
	" (" __DATE__ " - " __TIME__ ") for " __MACH__

int xboot_version(void)
{
	return XBOOT_VERSION(XBOOT_MAJOY, XBOOT_MINIOR, XBOOT_PATCH);
}

const char * xboot_version_string(void)
{
	return XBOOT_VERSION_STRING(XBOOT_MAJOY, XBOOT_MINIOR, XBOOT_PATCH);
}

const char * xboot_banner_string(void)
{
	return XBOOT_BANNER_STRING(XBOOT_MAJOY, XBOOT_MINIOR, XBOOT_PATCH);
}
