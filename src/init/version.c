/*
 * init/version.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xconfigs.h>
#include <version.h>

#define	XBOOT_MAJOY		3
#define	XBOOT_MINIOR	0
#define	XBOOT_PATCH		0

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
 *  \ \/ /|  _  |  _  |  _  |_   _|  (C) 2007-2018
 *   )  ( | |_| | |_| | |_| | | |____JIANJUN.JIANG__
 *  /_/\_\|_____|_____|_____| |_____________________|
 */
const char * xboot_character_logo_string(int i)
{
	static const char clogo[5][50] = {
		"       _                   _                     ",
		" _  _ | |___ _____ _____ _| |_                   ",
		"\\ \\/ /|  _  |  _  |  _  |_   _|  (C) 2007-2018   ",
		" )  ( | |_| | |_| | |_| | | |____JIANJUN.JIANG__ ",
		"/_/\\_\\|_____|_____|_____| |_____________________|",
	};

	if(i < 0 && i > 4)
		return "";
	return &clogo[i][0];
}
