/*
 * kernel/command/cmd-nettest.c
 */

#include <xboot.h>
#include <lan91c111.h>
#include <tcpip.h>
#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    nettest [args ...]\r\n");
}

static struct timer_t timer;

static int tcp_timer(struct timer_t *timer, void *data)
{
	TCPClockHandler();
	timer_forward(timer, ms_to_ktime(2));
	return 1;
}

static unsigned char *PWebSide;      	// pointer to webside
static unsigned int HTTPBytesToSend;	// bytes left to send

static unsigned char HTTPStatus;   		// status byte
#define HTTP_SEND_PAGE		0x01		// help flag

static const unsigned char GetResponse[] =              // 1st thing our server sends to a client
        { "HTTP/1.0 200 OK\r\n"                          // protocol ver 1.0, code 200, reason OK
		                "Content-Type: text/html\r\n"// type of data we want to send
		                "\r\n"                                         // indicate end of HTTP-header
        };

static const unsigned char WebSide[] = { "<html>\r\n"
		"<head>\r\n"
		"<meta http-equiv=\"refresh\" content=\"5\">\r\n"
		"<title>easyWEB - dynamic Webside</title>\r\n"
		"</head>\r\n"
		"\r\n"
		"<body bgcolor=\"#3030A0\" text=\"#FFFF00\">\r\n"
		"<p><b><font color=\"#FFFFFF\" size=\"6\"><i>Hello World!</i></font></b></p>\r\n"
		"\r\n"
		"<p><b>This is a dynamic webside hosted by the embedded Webserver</b> <b>easyWEB.</b></p>\r\n"
		"<p><b>Hardware:</b></p>\r\n"
		"<ul>\r\n"
		"<li><b>Phytec phyCore LPC229x board, 60 MHz, 2MB Flash, 1 MB SRAM</b></li>\r\n"
		"<li><b>SMSC 91C111 Ethernet Controller</b></li>\r\n"
		"</ul>\r\n"
		"\r\n"
		"<p><b>A/D Converter Input 0:</b></p>\r\n"
		"\r\n"
		"<table bgcolor=\"#ff0000\" border=\"5\" cellpadding=\"0\" cellspacing=\"0\" width=\"500\">\r\n"
		"<tr>\r\n"
		"<td>\r\n"
		"<table width=\"AD7%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n"
		"<tr><td bgcolor=\"#00ff00\"> </td></tr>\r\n"
		"</table>\r\n"
		"</td>\r\n"
		"</tr>\r\n"
		"</table>\r\n"
		"\r\n"
		"<table border=\"0\" width=\"540\">\r\n"
		"<tr>\r\n"
		"<td width=\"14%\">0V</td>\r\n"
		"<td width=\"14%\">0.5V</td>\r\n"
		"<td width=\"14%\">1V</td>\r\n"
		"<td width=\"14%\">1.5V</td>\r\n"
		"<td width=\"14%\">2V</td>\r\n"
		"<td width=\"14%\">2.5V</td>\r\n"
		"<td width=\"14%\">3V</td>\r\n"
		"</tr>\r\n"
		"</table>\r\n"
		"\r\n"
		"<p><b>A/D Converter Input 1:</b></p>\r\n"
		"\r\n"
		"<table bgcolor=\"#ff0000\" border=\"5\" cellpadding=\"0\" cellspacing=\"0\" width=\"500\">\r\n"
		"<tr>\r\n"
		"<td>\r\n"
		"<table width=\"ADA%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\r\n"
		"<tr><td bgcolor=\"#00ff00\"> </td></tr> \r\n"
		"</table>\r\n"
		"</td>\r\n"
		"</tr>\r\n"
		"</table>\r\n"
		"\r\n"
		"<table border=\"0\" width=\"540\">\r\n"
		"<tr>\r\n"
		"<td width=\"12%\">0V</td>\r\n"
		"<td width=\"12%\">0.5V</td>\r\n"
		"<td width=\"12%\">1V</td>\r\n"
		"<td width=\"12%\">1.5V</td>\r\n"
		"<td width=\"12%\">2V</td>\r\n"
		"<td width=\"12%\">2.5V</td>\r\n"
		"<td width=\"12%\">3V</td>\r\n"
		"</tr>\r\n"
		"</table>\r\n"
		"</body>\r\n"
		"</html>\r\n"
		"\r\n" };

static void HTTPServer(void)
{
	if(SocketStatus & SOCK_CONNECTED)             // check if somebody has connected to our TCP
	{
		if(SocketStatus & SOCK_DATA_AVAILABLE)
		{
			TCPReleaseRxBuffer();                      // and throw it away
		}
		if(SocketStatus & SOCK_TX_BUF_RELEASED)     // check if buffer is free for TX
		{
			if(!(HTTPStatus & HTTP_SEND_PAGE))        // init byte-counter and pointer to webside
			{                                          // if called the 1st time
				HTTPBytesToSend = sizeof(WebSide) - 1;   // get HTML length, ignore trailing zero
				PWebSide = (unsigned char*)WebSide;     // pointer to HTML-code
			}

			if(HTTPBytesToSend > MAX_TCP_TX_DATA_SIZE)     // transmit a segment of MAX_SIZE
			{
				if(!(HTTPStatus & HTTP_SEND_PAGE))           // 1st time, include HTTP-header
				{
					memcpy(TCP_TX_BUF, GetResponse, sizeof(GetResponse) - 1);
					memcpy(TCP_TX_BUF + sizeof(GetResponse) - 1, PWebSide, MAX_TCP_TX_DATA_SIZE - sizeof(GetResponse) + 1);
					HTTPBytesToSend -= MAX_TCP_TX_DATA_SIZE - sizeof(GetResponse) + 1;
					PWebSide += MAX_TCP_TX_DATA_SIZE - sizeof(GetResponse) + 1;
				}
				else
				{
					memcpy(TCP_TX_BUF, PWebSide, MAX_TCP_TX_DATA_SIZE);
					HTTPBytesToSend -= MAX_TCP_TX_DATA_SIZE;
					PWebSide += MAX_TCP_TX_DATA_SIZE;
				}

				TCPTxDataCount = MAX_TCP_TX_DATA_SIZE;   // bytes to xfer
				TCPTransmitTxBuffer();                   // xfer buffer
			}
			else if(HTTPBytesToSend)                  // transmit leftover bytes
			{
				memcpy(TCP_TX_BUF, PWebSide, HTTPBytesToSend);
				TCPTxDataCount = HTTPBytesToSend;        // bytes to xfer
				TCPTransmitTxBuffer();                   // send last segment
				TCPClose();                              // and close connection
				HTTPBytesToSend = 0;                     // all data sent
			}

			HTTPStatus |= HTTP_SEND_PAGE;              // ok, 1st loop executed
		}
	}
	else
		HTTPStatus &= ~HTTP_SEND_PAGE;               // reset help-flag if not connected
}

static int do_nettest(int argc, char ** argv)
{
	timer_init(&timer, tcp_timer, NULL);
	timer_start(&timer, ms_to_ktime(2));

	Init91111();
	HTTPStatus = 0;                                // clear HTTP-server's flag register

	TCPLocalPort = TCP_PORT_HTTP;                  // set port we want to listen to
	while(1)
	{
		if(!(SocketStatus & SOCK_ACTIVE))
			TCPPassiveOpen();
		DoNetworkStuff();
		HTTPServer();
		task_yield();
	}
	return 0;
}

static struct command_t cmd_nettest = {
	.name	= "nettest",
	.desc	= "debug command for programmer",
	.usage	= usage,
	.exec	= do_nettest,
};

static __init void nettest_cmd_init(void)
{
	register_command(&cmd_nettest);
}

static __exit void nettest_cmd_exit(void)
{
	unregister_command(&cmd_nettest);
}

command_initcall(nettest_cmd_init);
command_exitcall(nettest_cmd_exit);
