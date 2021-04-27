/*
 * driver/wifi/wifi-esp8266.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

#include <xboot.h>
#include <uart/uart.h>
#include <wifi/wifi.h>

enum recv_state_t {
	RECV_STATE_PLUS		= 0,
	RECV_STATE_I		= 1,
	RECV_STATE_P		= 2,
	RECV_STATE_D		= 3,
	RECV_STATE_COMMA	= 4,
	RECV_STATE_COUNT	= 5,
	RECV_STATE_BYTE		= 6,
	RECV_STATE_R		= 7,
	RECV_STATE_N		= 8,
};

enum at_resp_state_t {
	AT_RESP_STATE_O	= 0,
	AT_RESP_STATE_K	= 1,
	AT_RESP_STATE_R	= 2,
	AT_RESP_STATE_N	= 3,
};

struct wifi_esp8266_pdata_t {
	struct uart_t * uart;
	int baud;
	int data;
	int parity;
	int stop;

	ktime_t time;
	enum wifi_status_t status;
	enum recv_state_t rstate;
	char cbuf[16];
	int cidx;
	int count;
};

static int wifi_at_request(struct wifi_esp8266_pdata_t * pdat, const char * cmd, char * resp, int sz, int timeout)
{
	enum at_resp_state_t state = AT_RESP_STATE_O;
	ktime_t time;
	u8_t buf[256];
	int len = 0;
	int ok = 0;
	int l, n, i;
	char c;

	if(pdat)
	{
		if(cmd && ((l = strlen(cmd)) > 0))
			uart_write(pdat->uart, (const u8_t *)cmd, l);
		time = ktime_add_ms(ktime_get(), timeout);
		do {
			while((n = uart_read(pdat->uart, buf, sizeof(buf))) > 0)
			{
				for(i = 0; i < n; i++)
				{
					c = buf[i];
					switch(state)
					{
					case AT_RESP_STATE_O:
						if(c == 'O')
							state = AT_RESP_STATE_K;
						break;
					case AT_RESP_STATE_K:
						if(c == 'K')
							state = AT_RESP_STATE_R;
						else
							state = AT_RESP_STATE_O;
						break;
					case AT_RESP_STATE_R:
						if(c == '\r')
							state = AT_RESP_STATE_N;
						else
							state = AT_RESP_STATE_O;
						break;
					case AT_RESP_STATE_N:
						if(c == '\n')
							ok = 1;
						state = AT_RESP_STATE_O;
						break;
					default:
						state = AT_RESP_STATE_O;
						break;
					}
					if(resp && (len < sz - 1))
						resp[len++] = c;
				}
			}
			task_yield();
		} while(!ok && ktime_before(ktime_get(), time));
	}
	if(resp && (len < sz - 1))
		resp[len] = '\0';
	return ok ? len : 0;
}

static bool_t wifi_esp8266_join(struct wifi_t * wifi, const char * ssid, const char * passwd)
{
	struct wifi_esp8266_pdata_t * pdat = (struct wifi_esp8266_pdata_t *)wifi->priv;
	char cmd[256];
	char resp[256];

	pdat->status = WIFI_STATUS_CONNECTING;
	if(!uart_set(pdat->uart, pdat->baud, pdat->data, pdat->parity, pdat->stop))
		return FALSE;
	if(wifi_at_request(pdat, "AT\r\n", resp, sizeof(resp), 100) == 0)
		return FALSE;
	if(wifi_at_request(pdat, "AT+CWMODE=1\r\n", resp, sizeof(resp), 100) == 0)
		return FALSE;
	if(wifi_at_request(pdat, "AT+CWAUTOCONN=1\r\n", resp, sizeof(resp), 100) == 0)
		return FALSE;
	snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, passwd);
	if(wifi_at_request(pdat, cmd, resp, sizeof(resp), 10000) == 0)
		return FALSE;
	if(!strstr(resp, "WIFI GOT IP"))
		return FALSE;
	return TRUE;
}

static bool_t wifi_esp8266_exit(struct wifi_t * wifi)
{
	struct wifi_esp8266_pdata_t * pdat = (struct wifi_esp8266_pdata_t *)wifi->priv;
	char resp[256];

	pdat->status = WIFI_STATUS_DISCONNECTED;
	if(wifi_at_request(pdat, "AT+CWQAP\r\n", resp, sizeof(resp), 100) == 0)
		return FALSE;
	return TRUE;
}

static bool_t wifi_esp8266_connect(struct wifi_t * wifi, const char * ip, int port)
{
	struct wifi_esp8266_pdata_t * pdat = (struct wifi_esp8266_pdata_t *)wifi->priv;
	char cmd[256];
	char resp[256];

	pdat->status = WIFI_STATUS_CONNECTING;
	if(wifi_at_request(pdat, "AT+CIPMUX=0\r\n", resp, sizeof(resp), 100) > 0)
	{
		snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"%s\",\"%s\",%d\r\n", "TCP", ip, port);
		if(wifi_at_request(pdat, cmd, resp, sizeof(resp), 5000) > 0)
		{
			if((wifi_at_request(pdat, "AT+CIPSTATUS\r\n", resp, sizeof(resp), 100) > 0) && strstr(resp, "STATUS:3\r\n"))
			{
				pdat->status = WIFI_STATUS_CONNECTED;
				return TRUE;
			}
		}
	}
	pdat->status = WIFI_STATUS_DISCONNECTED;
	return FALSE;
}

static bool_t wifi_esp8266_disconnect(struct wifi_t * wifi)
{
	struct wifi_esp8266_pdata_t * pdat = (struct wifi_esp8266_pdata_t *)wifi->priv;
	char resp[256];

	pdat->status = WIFI_STATUS_DISCONNECTED;
	if(wifi_at_request(pdat, "AT+CIPCLOSE\r\n", resp, sizeof(resp), 100) == 0)
		return FALSE;
	return TRUE;
}

static enum wifi_status_t wifi_esp8266_status(struct wifi_t * wifi)
{
	struct wifi_esp8266_pdata_t * pdat = (struct wifi_esp8266_pdata_t *)wifi->priv;
	ktime_t now = ktime_get();
	char resp[256];

	if(pdat->status == WIFI_STATUS_CONNECTED)
	{
		if(ktime_after(now, ktime_add_ms(pdat->time, 5000)))
		{
			if((wifi_at_request(pdat, "AT+CIPSTATUS\r\n", resp, sizeof(resp), 100) == 0) || !strstr(resp, "STATUS:3\r\n"))
				pdat->status = WIFI_STATUS_DISCONNECTED;
			pdat->time = now;
		}
	}
	return pdat->status;
}

static int wifi_esp8266_read(struct wifi_t * wifi, void * buf, int count)
{
	struct wifi_esp8266_pdata_t * pdat = (struct wifi_esp8266_pdata_t *)wifi->priv;
	char * p = buf;
	int len = 0;
	u8_t c;

	if(pdat->status == WIFI_STATUS_CONNECTED)
	{
		while(count > 0)
		{
			if(uart_read(pdat->uart, &c, 1) == 1)
			{
				switch(pdat->rstate)
				{
				case RECV_STATE_PLUS:
					if(c == '+')
						pdat->rstate = RECV_STATE_I;
					else
						pdat->rstate = RECV_STATE_PLUS;
					break;
				case RECV_STATE_I:
					if(c == 'I')
						pdat->rstate = RECV_STATE_P;
					else
						pdat->rstate = RECV_STATE_PLUS;
					break;
				case RECV_STATE_P:
					if(c == 'P')
						pdat->rstate = RECV_STATE_D;
					else
						pdat->rstate = RECV_STATE_PLUS;
					break;
				case RECV_STATE_D:
					if(c == 'D')
						pdat->rstate = RECV_STATE_COMMA;
					else
						pdat->rstate = RECV_STATE_PLUS;
					break;
				case RECV_STATE_COMMA:
					if(c == ',')
					{
						pdat->cidx = 0;
						pdat->rstate = RECV_STATE_COUNT;
					}
					else
						pdat->rstate = RECV_STATE_PLUS;
					break;
				case RECV_STATE_COUNT:
					if(isdigit(c))
					{
						pdat->cbuf[pdat->cidx++] = c;
					}
					else if(c == ':')
					{
						pdat->cbuf[pdat->cidx++] = '\0';
						pdat->count = strtoul(pdat->cbuf, NULL, 0);
						pdat->rstate = RECV_STATE_BYTE;
					}
					else
					{
						pdat->cidx = 0;
						pdat->rstate = RECV_STATE_PLUS;
					}
					break;
				case RECV_STATE_BYTE:
					*p++ = c;
					len++;
					count--;
					pdat->count--;
					if(pdat->count <= 0)
						pdat->rstate = RECV_STATE_R;
					break;
				case RECV_STATE_R:
					if(c == '\r')
						pdat->rstate = RECV_STATE_N;
					else
						pdat->rstate = RECV_STATE_PLUS;
					break;
				case RECV_STATE_N:
					pdat->rstate = RECV_STATE_PLUS;
					break;
				default:
					pdat->rstate = RECV_STATE_PLUS;
					break;
				}
			}
			else
				break;
		}
	}
	return len;
}

int wifi_esp8266_write(struct wifi_t * wifi, void * buf, int count)
{
	struct wifi_esp8266_pdata_t * pdat = (struct wifi_esp8266_pdata_t *)wifi->priv;
	ktime_t time;
	char cmd[256];
	char resp[256];
	int len;
	u8_t c;

	if(pdat->status == WIFI_STATUS_CONNECTED)
	{
		if(buf && (count > 0))
		{
			len = snprintf(cmd, sizeof(cmd), "AT+CIPSEND=%d\r\n", count);
			uart_write(pdat->uart, (const u8_t *)cmd, len);
			time = ktime_add_ms(ktime_get(), 100);
			do {
				if((uart_read(pdat->uart, &c, 1) == 1) && (c == '>'))
				{
					len = uart_write(pdat->uart, (const u8_t *)buf, count);
					if(len > 0)
					{
						if(wifi_at_request(pdat, NULL, resp, sizeof(resp), 1000) && strstr(resp, "SEND OK"))
							return len;
					}
				}
				task_yield();
			} while(ktime_before(ktime_get(), time));
		}
	}
	return 0;
}

static int wifi_esp8266_ioctl(struct wifi_t * wifi, const char * cmd, void * arg)
{
	return -1;
}

static struct device_t * wifi_esp8266_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct wifi_esp8266_pdata_t * pdat;
	struct wifi_t * wifi;
	struct device_t * dev;
	struct uart_t * uart = search_uart(dt_read_string(n, "uart-bus", NULL));

	if(!uart)
		return NULL;

	pdat = malloc(sizeof(struct wifi_esp8266_pdata_t));
	if(!pdat)
		return NULL;

	wifi = malloc(sizeof(struct wifi_t));
	if(!wifi)
	{
		free(pdat);
		return NULL;
	}
	memset(wifi, 0, sizeof(struct wifi_t));

	pdat->uart = uart;
	pdat->baud = dt_read_int(n, "baud-rates", 115200);
	pdat->data = dt_read_int(n, "data-bits", 8);
	pdat->parity = dt_read_int(n, "parity-bits", 0);
	pdat->stop = dt_read_int(n, "stop-bits", 1);
	pdat->time = ktime_get();
	pdat->status = WIFI_STATUS_DISCONNECTED;
	pdat->rstate = RECV_STATE_PLUS;
	pdat->cidx = 0;
	pdat->count = 0;

	wifi->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	wifi->join = wifi_esp8266_join;
	wifi->exit = wifi_esp8266_exit;
	wifi->connect = wifi_esp8266_connect;
	wifi->disconnect = wifi_esp8266_disconnect;
	wifi->status = wifi_esp8266_status;
	wifi->read = wifi_esp8266_read;
	wifi->write = wifi_esp8266_write;
	wifi->ioctl = wifi_esp8266_ioctl;
	wifi->priv = pdat;

	if(!(dev = register_wifi(wifi, drv)))
	{
		free_device_name(wifi->name);
		free(wifi->priv);
		free(wifi);
		return NULL;
	}
	return dev;
}

static void wifi_esp8266_remove(struct device_t * dev)
{
	struct wifi_t * wifi = (struct wifi_t *)dev->priv;

	if(wifi)
	{
		unregister_wifi(wifi);
		free_device_name(wifi->name);
		free(wifi->priv);
		free(wifi);
	}
}

static void wifi_esp8266_suspend(struct device_t * dev)
{
}

static void wifi_esp8266_resume(struct device_t * dev)
{
}

static struct driver_t wifi_esp8266 = {
	.name		= "wifi-esp8266",
	.probe		= wifi_esp8266_probe,
	.remove		= wifi_esp8266_remove,
	.suspend	= wifi_esp8266_suspend,
	.resume		= wifi_esp8266_resume,
};

static __init void wifi_esp8266_driver_init(void)
{
	register_driver(&wifi_esp8266);
}

static __exit void wifi_esp8266_driver_exit(void)
{
	unregister_driver(&wifi_esp8266);
}

driver_initcall(wifi_esp8266_driver_init);
driver_exitcall(wifi_esp8266_driver_exit);
