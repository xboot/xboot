/*
 * kernel/command/cmd-net.c
 */

#include <xboot.h>
#include <net/net.h>
#include <command/command.h>

static void usage(void)
{
	struct device_t * pos, * n;

	printf("usage:\r\n");
	printf("    net <device> server <type> <port>        - Listen port for waiting connection\r\n");
	printf("    net <device> client <type> <host> <port> - Connect to the remote host port\r\n");

	printf("supported device list:\r\n");
	list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_NET], head)
	{
		printf("    %s\r\n", pos->name);
	}
}

static int do_net(int argc, char ** argv)
{
	struct net_t * net;

	if(argc < 3)
	{
		usage();
		return -1;
	}
	net = search_net(argv[1]);
	if(net)
	{
		if(!strcmp(argv[2], "server"))
		{
/*			argc -= 3;
			argv += 3;
			if(argc == 2)
			{
				struct socket_listen_t * l = net_listen(net, argv[0], atoi(argv[1]));
				if(l)
				{
					struct socket_connect_t * c;
					printf("listen ok\r\n");

					while(1)
					{
						c = net_accept(l);
						if(c)
						{
							printf("net_accept ok\r\n");
							char buf[256];
							while(1)
							{
								printf("net_read \r\n");
								int len = net_read(c, buf, sizeof(buf));
								if(len > 0)
								{
									for(int i = 0; i < len; i++)
									{
										if(isprint(buf[i]) || (buf[i] == '\r') || (buf[i] == '\n') || (buf[i] == '\t') || (buf[i] == '\f'))
											putchar(buf[i]);
										else
											putchar('.');
									}
									printf("len = %d\r\n", len);
								}
								task_yield();
							}
						}
						task_yield();
					}
				}
				else
					printf("Failed to listen '%s' with '%s' type\r\n", argv[1], argv[0]);
			}*/
		}
		else if(!strcmp(argv[2], "client"))
		{
			argc -= 3;
			argv += 3;
			if(argc == 3)
			{
				struct socket_connect_t * c = net_connect(net, argv[0], argv[1], atoi(argv[2]));
				if(c)
				{
					char buf[SZ_4K];
					while(1)
					{
						if(!net_status(c))
							break;
						int ch = getchar();
						if(ch != EOF)
						{
							if(ch == 0x3)
								break;
							unsigned char uc = ch;
							net_write(c, &uc, 1);
							printf("%c", uc);
						}
						int len = net_read(c, buf, sizeof(buf));
						if(len > 0)
						{
							for(int i = 0; i < len; i++)
							{
								if(isprint(buf[i]) || (buf[i] == '\r') || (buf[i] == '\n') || (buf[i] == '\t') || (buf[i] == '\f'))
									putchar(buf[i]);
								else
									putchar('.');
							}
						}
						task_yield();
					}
					net_close(c);
				}
				else
					printf("Failed to connect '%s:%s' with '%s' type\r\n", argv[1], argv[2], argv[0]);
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

static struct command_t cmd_net = {
	.name	= "net",
	.desc	= "network protocol tool",
	.usage	= usage,
	.exec	= do_net,
};

static __init void net_cmd_init(void)
{
	register_command(&cmd_net);
}

static __exit void net_cmd_exit(void)
{
	unregister_command(&cmd_net);
}

command_initcall(net_cmd_init);
command_exitcall(net_cmd_exit);
