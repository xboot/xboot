/*
 * kernel/command/cmd-net.c
 */

#include <xboot.h>
#include <net/net.h>
#include <command/command.h>

struct srl_buf_t {
	char buf[SZ_1K];
	int len;
};

static int simple_readline(struct srl_buf_t * srl)
{
	int ch;

	if((ch = getchar()) != EOF)
	{
		unsigned char c = ch;
		switch(c)
		{
		case 0x3:
			return -1;
		case 0xd:
			if(srl->len < sizeof(srl->buf) - 1)
			{
				srl->buf[srl->len++] = '\r';
				srl->buf[srl->len++] = '\n';
				printf("\r\n");
			}
			return 1;
		default:
			if(srl->len < sizeof(srl->buf))
			{
				srl->buf[srl->len++] = c;
				printf("%c", c);
			}
			break;
		}
	}
	return 0;
}

static void usage(void)
{
	struct device_t * pos, * n;

	printf("usage:\r\n");
	printf("    net <device> server <type> <port>        - Listen port for waiting connection\r\n");
	printf("    net <device> client <type> <host> <port> - Connect to the remote server\r\n");

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
					char buf[SZ_1K];
					struct srl_buf_t srl;
					srl.len = 0;
					while(1)
					{
						if(!net_status(c))
							break;
						int r = simple_readline(&srl);
						if(r > 0)
						{
							net_write(c, srl.buf, srl.len);
							srl.len = 0;
						}
						else if(r < 0)
							break;
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
					printf("Failed to connect server '%s:%s' with '%s' type\r\n", argv[1], argv[2], argv[0]);
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
	.desc	= "network protocol debug tool",
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
