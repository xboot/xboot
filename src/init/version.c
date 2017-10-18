/*
 * init/version.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
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

#include <xconfigs.h>
#include <version.h>

#define	XBOOT_MAJOY		2
#define	XBOOT_MINIOR	0
#define	XBOOT_PATCH		2

#define VERSION_TO_STRING(major, minor, patch) \
	#major"."#minor"."#patch

#define XBOOT_VERSION_STRING(major, minor, patch) \
	VERSION_TO_STRING(major, minor, patch)

int xboot_version(void)
{
	return (XBOOT_MAJOY * 100) + (XBOOT_MINIOR * 10) + (XBOOT_PATCH * 1);
}

const char * xboot_version_string(void)
{
	return XBOOT_VERSION_STRING(XBOOT_MAJOY, XBOOT_MINIOR, XBOOT_PATCH);
}

const char * xboot_banner_string(void)
{
	return ("V"XBOOT_VERSION_STRING(XBOOT_MAJOY, XBOOT_MINIOR, XBOOT_PATCH)" ("__DATE__" - "__TIME__")");
}

/*
 *         _                   _
 *   _  _ | |___ _____ _____ _| |_
 *  \ \/ /|  _  |  _  |  _  |_   _|  (C) 2007-2017
 *   )  ( | |_| | |_| | |_| | | |____JIANJUN.JIANG__
 *  /_/\_\|_____|_____|_____| |_____________________|
 */
const char * xboot_character_logo_string(int i)
{
	static const char clogo[5][50] = {
		"       _                   _                     ",
		" _  _ | |___ _____ _____ _| |_                   ",
		"\\ \\/ /|  _  |  _  |  _  |_   _|  (C) 2007-2017   ",
		" )  ( | |_| | |_| | |_| | | |____JIANJUN.JIANG__ ",
		"/_/\\_\\|_____|_____|_____| |_____________________|",
	};

	if(i < 0 && i > 4)
		return "";
	return &clogo[i][0];
}
