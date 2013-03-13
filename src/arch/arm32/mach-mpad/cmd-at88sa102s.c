/*
 * cmd-at88sa102s.c
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
#include <mpad-at88sa102s.h>
#include <command/command.h>

static void usage(void)
{
	printk("usage:\r\n    at88sa102s [auth | info | burn]\r\n");
}

static u8_t key[8] = {
	0x00, 0x11, 0x22, 0x33,	0x44, 0x55, 0x66, 0x77,
};

static u8_t fuse[3] = {
	0x88, 0x99, 0x00,
};

static int do_at88sa102s(int argc, char ** argv)
{
	u8_t buf[128];
	u8_t sn[6];
	int i;

	if(argc < 2)
	{
		usage();
		return -1;
	}

	for(i = 1; i < argc; i++)
	{
		if( !strcmp((const char *)argv[i], "auth"))
		{
			/*
			 * setting id
			 */
			if(! sa_read_serial_number(sn))
				memset(sn, 0, sizeof(sn));

			sprintf((char *)buf, "id=0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x", sn[0], sn[1], sn[2], sn[3], sn[4], sn[5]);
			putenv((const char *)buf);

			/*
			 * setting verify
			 */
			if(sa_do_auth())
			{
				putenv("verify=ok");
				printk("ok\r\n");
			}
			else
			{
				putenv("verify=fail");
				printk("fail\r\n");
			}
		}
		else if( !strcmp((const char *)argv[i], "info"))
		{
			printk("sn: ");
			if(sa_read_serial_number(buf))
			{
				for(i=0; i<6; i++)
				{
					printk("0x%02x,", buf[i]);
				}
			}
			printk("\r\n");

			printk("manufacture id: ");
			if(sa_read_manufacture_id(buf))
			{
				for(i=0; i<3; i++)
				{
					printk("0x%02x,", buf[i]);
				}
			}
			printk("\r\n");

			printk("revision: ");
			if(sa_read_revision(buf))
			{
				for(i=0; i<4; i++)
				{
					printk("0x%02x,", buf[i]);
				}
			}
			printk("\r\n");

			printk("fuse status: ");
			if(sa_read_fuse_status(buf))
			{
				for(i=0; i<3; i++)
				{
					printk("0x%02x,", buf[i]);
				}
			}
			printk("\r\n");
		}
		else if( !strcmp((const char *)argv[i], "burn"))
		{
			if(sa_burn_secure(key, fuse))
				printk("burn ok\r\n");
			else
				printk("burn fail\r\n");
		}
		else
		{
			usage();
			return -1;
		}
	}

	return 0;
}

static struct command at88sa102s_cmd = {
	.name		= "at88sa102s",
	.func		= do_at88sa102s,
	.desc		= "AT88SA102S crypto chip\r\n",
	.usage		= "at88sa102s [auth | info | burn]\r\n",
	.help		= "    at88sa102s crypto chip command\r\n"
};

static __init void at88sa102s_cmd_init(void)
{
	if(!command_register(&at88sa102s_cmd))
		LOG_E("register 'at88sa102s' command fail");
}

static __exit void at88sa102s_cmd_exit(void)
{
	if(!command_unregister(&at88sa102s_cmd))
		LOG_E("unregister 'at88sa102s' command fail");
}

command_initcall(at88sa102s_cmd_init);
command_exitcall(at88sa102s_cmd_exit);
