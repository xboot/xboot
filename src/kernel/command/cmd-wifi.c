/*
 * kernel/command/cmd-wifi.c
 */

#include <xboot.h>
#include <wifi/wifi.h>
#include <command/command.h>

static void usage(void)
{
	struct device_t * pos, * n;

	printf("usage:\r\n");
	printf("    wifi <device> connect <ssid> <passwd> - Connect to wifi access point\r\n");
	printf("    wifi <device> disconnect              - Disconnect from wifi access point\r\n");
	printf("    wifi <device> client open <ip> <port> - Open tcp client\r\n");
	printf("    wifi <device> client close            - Close tcp client\r\n");
	printf("    wifi <device> bypass open             - Open bypass mode\r\n");
	printf("    wifi <device> bypass close            - Close bypass mode\r\n");
	printf("    wifi <device> write <data>            - Write data to server\r\n");
	printf("    wifi <device> read <timeout>          - Read data from server\r\n");
	printf("    wifi <device> info                    - Show wifi information\r\n");

	printf("supported device list:\r\n");
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_WIFI], head)
	{
		printf("    %s\r\n", pos->name);
	}
}

static int do_wifi(int argc, char ** argv)
{
	struct wifi_t * wifi;
	char buf[256];

	if(argc > 2)
	{
		wifi = search_wifi(argv[1]);
		if(wifi)
		{
			if(!strcmp(argv[2], "connect"))
			{
				argc -= 3;
				argv += 3;
				if(argc == 2)
				{
					if(!wifi_connect(wifi, argv[0], argv[1]))
						printf("Failed to connect wifi access point\r\n");
				}
				else
					usage();
			}
			else if(!strcmp(argv[2], "disconnect"))
			{
				if(!wifi_disconnect(wifi))
					printf("Failed to disconnect wifi access point\r\n");
			}
			else if(!strcmp(argv[2], "client"))
			{
				argc -= 3;
				argv += 3;
				if(argc >= 1)
				{
					if(!strcmp(argv[0], "open"))
					{
						if(argc >= 3)
						{
							if(!wifi_client_open(wifi, argv[1], strtol(argv[2], NULL, 0)))
								printf("Failed to open tcp client\r\n");
						}
						else
							usage();
					}
					else if(!strcmp(argv[0], "close"))
					{
						if(!wifi_client_close(wifi))
							printf("Failed to close tcp client\r\n");
					}
					else
						usage();
				}
				else
					usage();
			}
			else if(!strcmp(argv[2], "bypass"))
			{
				argc -= 3;
				argv += 3;
				if(argc == 1)
				{
					if(!strcmp(argv[0], "open"))
					{
						if(wifi_ioctl(wifi, "wifi-open-bypass", NULL) < 0)
							printf("Failed to open bypass mode\r\n");
					}
					else if(!strcmp(argv[0], "close"))
					{
						if(wifi_ioctl(wifi, "wifi-close-bypass", NULL) < 0)
							printf("Failed to close bypass mode\r\n");
					}
					else
						usage();
				}
				else
					usage();
			}
			else if(!strcmp(argv[2], "write"))
			{
				argc -= 3;
				argv += 3;
				if(argc == 1)
					wifi_write(wifi, argv[0], strlen(argv[0]));
				else
					usage();
			}
			else if(!strcmp(argv[2], "read"))
			{
				argc -= 3;
				argv += 3;
				if(argc == 1)
				{
					ktime_t time = ktime_add_ms(ktime_get(), strtol(argv[0], NULL, 0) * 1000);
					do {
						int len = wifi_read(wifi, buf, sizeof(buf));
						for(int i = 0; i < len; i++)
						{
							if(isprint(buf[i]) || (buf[i] == '\r') || (buf[i] == '\n') || (buf[i] == '\t') || (buf[i] == '\f'))
								putchar(buf[i]);
							else
								putchar('.');
						}
					} while(ktime_before(ktime_get(), time));
				}
				else
					usage();
			}
			else if(!strcmp(argv[2], "info"))
			{
				printf("wifi status : ");
				switch(wifi_status(wifi))
				{
				case WIFI_STATUS_DISCONNECTED:
					printf("disconnected\r\n");
					break;
				case WIFI_STATUS_CONNECTING:
					printf("connecting\r\n");
					break;
				case WIFI_STATUS_CONNECTED:
					printf("connected\r\n");
					break;
				case WIFI_STATUS_CLIENT_OPENING:
					printf("client opening\r\n");
					break;
				case WIFI_STATUS_CLIENT_OPENED:
					printf("client opened\r\n");
					break;
				default:
					break;
				}
				if(wifi_ioctl(wifi, "wifi-get-ip", buf) == 0)
					printf("wifi ip address : %s\r\n", buf);
				if(wifi_ioctl(wifi, "wifi-get-mac", buf) == 0)
					printf("wifi mac address : %s\r\n", buf);
			}
			else
				usage();
		}
		else
			usage();
	}
	else
		usage();
	return 0;
}

static struct command_t cmd_wifi = {
	.name	= "wifi",
	.desc	= "configure wireless interface",
	.usage	= usage,
	.exec	= do_wifi,
};

static __init void wifi_cmd_init(void)
{
	register_command(&cmd_wifi);
}

static __exit void wifi_cmd_exit(void)
{
	unregister_command(&cmd_wifi);
}

command_initcall(wifi_cmd_init);
command_exitcall(wifi_cmd_exit);
