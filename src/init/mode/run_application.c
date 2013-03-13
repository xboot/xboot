/*
 * init/mode/run_graphic.c
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


#include <xboot.h>
#include <stddef.h>
#include <mode/mode.h>

/*
 * default application for this mode
 */
static void default_application(void)
{
	do {
		/*
		 * enter to shell mode
		 */
		xboot_set_mode(MODE_SHELL);

	} while(xboot_get_mode() == MODE_APPLICATION);
}

static application_t xboot_application = default_application;

/*
 * register application
 */
bool_t register_application(application_t app)
{
	if(app)
	{
		xboot_application = app;
		return TRUE;
	}
	else
	{
		xboot_application = default_application;
		return FALSE;
	}
}

/*
 * running the application mode
 */
void run_application_mode(void)
{
	if(xboot_application)
		xboot_application();
}
