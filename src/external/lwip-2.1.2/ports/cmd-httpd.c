/*
 * kernel/command/cmd-httpd.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any lahttpdsion.
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

#include <command/command.h>
#include <lwip/tcp.h>
#include <lwip/etharp.h>
#include <lwip/init.h>
#include <lwip/priv/tcp_priv.h>
#include <netif/ethernet.h>

#define STR_AND_SIZE(str)	(str), strlen(str)

static err_t http_recv(void * arg, struct tcp_pcb * tpcb, struct pbuf * p, err_t err)
{
    char len[10];
    char str[200];
    char name[100];
    char * pstr;
    uint8_t i = 0;
    if(p != NULL)
    {
        pstr = (char *)p->payload;
        while (*pstr++ != ' ');
        while (*pstr != ' ')
            name[i++] = *pstr++;
        name[i] = '\0';
        tcp_recved(tpcb, p->tot_len);

        sprintf(str, "<meta charset=\"utf8\"><title>获取网页名称</title><div style=\"font-family:Arial\"><b>请求的网页文件名称是: </b>%s</div>", name);

        sprintf(len, "%d", strlen(str));
        tcp_write(tpcb, STR_AND_SIZE("HTTP/1.1 200 OK\r\nContent-Length: "), TCP_WRITE_FLAG_MORE);
        tcp_write(tpcb, STR_AND_SIZE(len), TCP_WRITE_FLAG_COPY | TCP_WRITE_FLAG_MORE);
        tcp_write(tpcb, STR_AND_SIZE("\r\nKeep-Alive: timeout=5, max=100\r\nConnection: Keep-Alive\r\nContent-Type: text/html\r\n\r\n"), TCP_WRITE_FLAG_MORE);
        tcp_write(tpcb, STR_AND_SIZE(str), TCP_WRITE_FLAG_COPY);
        pbuf_free(p);
    }
    return ERR_OK;
}

static err_t http_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    tcp_recv(newpcb, http_recv);
    return ERR_OK;
}

static void http_init(void)
{
    struct tcp_pcb * tpcb = tcp_new();
    tcp_bind(tpcb, IP_ADDR_ANY, 80);
    tpcb = tcp_listen(tpcb);
    tcp_accept(tpcb, http_accept);
}

static void usage(void)
{
	printf("usage:\r\n");
	printf("    httpd web server\r\n");
}

static int do_httpd(int argc, char ** argv)
{
    struct ip4_addr ipaddr, netmask, gw;
    struct netif enc28j60;
    int cnt = 0;
    ktime_t timeout;

	lwip_init();
	IP4_ADDR(&ipaddr, 192, 168, 0, 55);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw, 192, 168, 0, 1);

//	netif_add(&enc28j60, &ipaddr, &netmask, &gw, NULL, ethernetif_init, ethernet_input);
	netif_set_default(&enc28j60);
	netif_set_up(&enc28j60);

    http_init();
    timeout = ktime_add_ms(ktime_get(), 250);
    while(1)
    {
/*		if(ENC28J60_GetPacketNum() != 0)
		{
			ethernetif_input(&enc28j60);
		}*/

		if(ktime_after(ktime_get(), timeout))
		{
			timeout = ktime_add_ms(ktime_get(), 250);
			cnt++;
			if(cnt >= 20)
			{
				cnt = 0;
				etharp_tmr();
			}
			tcp_tmr();
		}
    }

	return 0;
}

static struct command_t cmd_httpd = {
	.name	= "httpd",
	.desc	= "run httpd server",
	.usage	= usage,
	.exec	= do_httpd,
};

static __init void httpd_cmd_init(void)
{
	register_command(&cmd_httpd);
}

static __exit void httpd_cmd_exit(void)
{
	unregister_command(&cmd_httpd);
}

command_initcall(httpd_cmd_init);
command_exitcall(httpd_cmd_exit);
