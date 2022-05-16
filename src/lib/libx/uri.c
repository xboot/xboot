/*
 * libx/uri.c
 */

#include <types.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <uri.h>

int uri_parse(const char * s, struct uri_t * uri)
{
	if(s && uri)
	{
		int r = 1, i = 0, state = 0, off = 0, len = 0, ipv6 = 0;
		char c = s[0];

		memset(uri, 0, sizeof(struct uri_t));
		uri->scheme = uri->buf;
		while(((i + off) < (sizeof(uri->buf) - 1)) && c && r)
		{
			len += 1;
			switch(state)
			{
			case 0:
				if(c == ':')
				{
					c = 0;
					len = 0;
					state = 1;
				}
				break;
			case 1:
				if(len == 1)
				{
					if(c == '?')
					{
						state = 6;
						continue;
					}
					else if(c == '#')
					{
						state = 7;
						continue;
					}
					else if(c != '/')
					{
						r = 0;
					}
				}
				else if(len == 2)
				{
					if(c != '/')
					{
						uri->path = &uri->buf[i + off - 1];
						state = 6;
						continue;
					}
				}
				else if(len == 3)
				{
					if(c == '/')
					{
						len = 0;
						state = 5;
						continue;
					}
					else if(c == '[')
					{
						len = 0;
						state = 4;
						uri->host = &uri->buf[i + off];
						continue;
					}
					else
					{
						len = 0;
						state = 2;
						uri->user = &uri->buf[i + off];
					}
				}
				break;
			case 2:
				if(c == ':')
				{
					c = 0;
					len = 0;
					state = 3;
					uri->pass = &uri->buf[i + off + 1];
				}
				else if(c == '@')
				{
					state = 3;
					continue;
				}
				else if(c == '/')
				{
					state = 5;
					continue;
				}
				else if(c == '?')
				{
					state = 6;
					continue;
				}
				else if(c == '#')
				{
					state = 7;
					continue;
				}
				break;
			case 3:
				if(c == '@')
				{
					c = 0;
					len = 0;
					state = 4;
					uri->host = &uri->buf[i + off + 1];
				}
				else if(c == '/')
				{
					uri->host = uri->user;
					uri->user = NULL;
					uri->port = uri->pass;
					uri->pass = NULL;
					state = 5;
					continue;
				}
				else if(c == '?')
				{
					state = 6;
					continue;
				}
				else if(c == '#')
				{
					state = 7;
					continue;
				}
				break;
			case 4:
				if((len == 1) && (c == '['))
				{
					ipv6 = 1;
				}
				else if(ipv6 && (3 < len) && (c == ']'))
				{
					ipv6 = 0;
				}
				if(!ipv6 && (c == ':'))
				{
					c = 0;
					len = 0;
					state = 5;
					uri->port = &uri->buf[i + off + 1];
				}
				else if(c == '/')
				{
					state = 5;
					continue;
				}
				else if(c == '?')
				{
					state = 6;
					continue;
				}
				else if(c == '#')
				{
					state = 7;
					continue;
				}
				break;
			case 5:
				if(c == '/')
				{
					uri->buf[i + off] = 0;
					off += 1;
					len = 1;
					state = 6;
					uri->path = &uri->buf[i + off];
				}
				else if(c == '?')
				{
					state = 6;
					continue;
				}
				else if(c == '#')
				{
					state = 7;
					continue;
				}
				break;
			case 6:
				if(c == '?')
				{
					c = 0;
					len = 0;
					state = 7;
					uri->query = &uri->buf[i + off + 1];
				}
				else if(c == '#')
				{
					state = 7;
					continue;
				}
				break;
			case 7:
				if(c == '#')
				{
					c = 0;
					len = 0;
					state = 8;
					uri->fragment = &uri->buf[i + off + 1];
				}
				break;
			default:
				break;
			}
			uri->buf[i + off] = c;
			i += 1;
			c = s[i];
		}
		if(!uri->host && uri->user)
		{
			uri->host = uri->user;
			uri->user = NULL;
			if(!uri->port && uri->pass)
			{
				uri->port = uri->pass;
				uri->pass = NULL;
			}
		}
		return r;
	}
	return 0;
}
