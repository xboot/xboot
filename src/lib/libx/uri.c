/*
 * libx/uri.c
 */

#include <types.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <uri.h>

struct uri_component_t {
	int has_scheme;
	int has_userpass;
	int has_host;
	int has_port;
};

static int uri_char_check(uint8_t c)
{
	static const uint32_t ctable[8] = {
		0x00000000,
		0xaffffffa,
		0xafffffff,
		0x47fffffe,
		0x00000000,
		0x00000000,
		0x00000000,
		0x00000000,
	};
	int n = c / 32;
	int m = c % 32;
	return ctable[n] & (1 << m);
}

static int uri_check(const char * str, int len, struct uri_component_t * comp)
{
	const char * pend;
	enum {
		URI_PARSE_START,
		URI_PARSE_SCHEME,
		URI_PARSE_AUTHORITY,
		URI_PARSE_USERPASS,
		URI_PARSE_HOST,
		URI_PARSE_PORT,
		URI_PARSE_HOST_IPV6,
	} state;
	char c;

	state = URI_PARSE_START;
	comp->has_scheme = 0;
	comp->has_userpass = 0;
	comp->has_host = 0;
	comp->has_port = 0;
	for(pend = str + len; str < pend; ++str)
	{
		c = *str;
		if(uri_char_check(c) == 0)
			return 0;
		switch(state)
		{
		case URI_PARSE_START:
			switch(c)
			{
			case '/':
				return 1;
			case '[':
				state = URI_PARSE_HOST_IPV6;
				comp->has_host = 1;
				break;
			default:
				state = URI_PARSE_SCHEME;
				comp->has_host = 1;
				--str;
				break;
			}
			break;
		case URI_PARSE_SCHEME:
			switch(c)
			{
			case ':':
				state = URI_PARSE_AUTHORITY;
				break;
			case '@':
				state = URI_PARSE_HOST;
				comp->has_userpass = 1;
				break;
			case '/':
			case '?':
			case '#':
				return 1;
			default:
				break;
			}
			break;
		case URI_PARSE_AUTHORITY:
			if(c == '/')
			{
				if((str + 1 < pend) && (str[1] == '/'))
				{
					state = URI_PARSE_HOST;
					comp->has_scheme = 1;
					str += 1;
				}
				else
				{
					comp->has_port = 1;
					return 1;
				}
			}
			else
			{
				comp->has_port = 1;
				state = URI_PARSE_PORT;
			}
			break;
		case URI_PARSE_HOST:
			switch(c)
			{
			case '@':
				comp->has_userpass = 1;
				break;
			case '[':
				state = URI_PARSE_HOST_IPV6;
				break;
			case ':':
				comp->has_port = 1;
				state = URI_PARSE_PORT;
				break;
			case '/':
			case '?':
			case '#':
				return 1;
			default:
				break;
			}
			break;
		case URI_PARSE_PORT:
			switch(c)
			{
			case '@':
				comp->has_port = 0;
				comp->has_userpass = 1;
				state = URI_PARSE_HOST;
				break;
			case '[':
			case ']':
			case ':':
				return 0;
			case '/':
			case '?':
			case '#':
				comp->has_port = 1;
				return 1;
			default:
				break;
			}
			break;
		case URI_PARSE_HOST_IPV6:
			switch(c)
			{
			case ']':
				state = URI_PARSE_HOST;
				break;
			case '@':
			case '[':
			case '/':
			case '?':
			case '#':
				return 0;
			default:
				break;
			}
			break;
		default:
			return 0;
		}
	}
	return 1;
}

static int uri_parse(struct uri_t * uri, const char * str, int len)
{
	struct uri_component_t items;
	const char * pend;
	char * p;

	if(!uri_check(str, len, &items))
		return 0;
	pend = str + len;
	p = (char *)(uri + 1);
	if(items.has_scheme)
	{
		uri->scheme = p;
		while((str < pend) && (*str != ':'))
			*p++ = *str++;
		*p++ = 0;
		str += 3;
	}
	else
		uri->scheme = NULL;
	if(items.has_userpass)
	{
		uri->userpass = p;
		while((str < pend) && (*str != '@'))
			*p++ = *str++;
		*p++ = 0;
		str += 1;
	}
	else
		uri->userpass = NULL;
	if(items.has_host)
	{
		uri->host = p;
		if('[' == *str)
		{
			++str;
			while((str < pend) && (*str != ']'))
				*p++ = *str++;
			*p++ = 0;
			str += 1;
			if((str < pend) && *str && !strchr(":/?#", *str))
				return 0;
		}
		else
		{
			while((str < pend) && *str && !strchr(":/?#", *str))
				*p++ = *str++;
			*p++ = 0;
		}
	}
	else
		uri->host = NULL;
	if(items.has_port)
	{
		++str;
		for(uri->port = 0; (str < pend) && (*str >= '0') && (*str <= '9'); str++)
			uri->port = uri->port * 10 + (*str - '0');
		if((str < pend) && *str && !strchr(":/?#", *str))
			return 0;
	}
	else
		uri->port = 0;
	uri->path = p;
	if((str < pend) && (*str == '/'))
	{
		while((str < pend) && *str && (*str != '?') && (*str != '#'))
			*p++ = *str++;
		*p++ = 0;
	}
	else
	{
		*p++ = '/';
		*p++ = 0;
	}
	if((str < pend) && (*str == '?'))
	{
		uri->query = p;
		for(++str; (str < pend) && *str && (*str != '#'); ++str)
			*p++ = *str;
		*p++ = 0;
	}
	else
		uri->query = NULL;
	if((str < pend) && (*str == '#'))
	{
		uri->fragment = p;
		while((str < pend) && *++str)
			*p++ = *str;
		*p++ = 0;
	}
	else
		uri->fragment = NULL;
	return 1;
}

struct uri_t * uri_alloc(const char * str)
{
	struct uri_t * uri;
	int len;

	if(!str || !*str || ((len = strlen(str)) < 1))
		return NULL;
	uri = (struct uri_t *)malloc(sizeof(struct uri_t) + len + 5);
	if(!uri)
		return NULL;
	if(!uri_parse(uri, str, len))
	{
		free(uri);
		return NULL;
	}
	return uri;
}

void uri_free(struct uri_t * uri)
{
	if(uri)
		free(uri);
}

int uri_path(struct uri_t * uri, char * buf, int len)
{
	int r, n;

	n = snprintf(buf, len, "%s", uri->path);
	if(n < 0 || n >= len)
		return 0;
	if(uri->query && *uri->query)
	{
		r = snprintf(buf + n, len - n, "?%s", uri->query);
		if(r < 0 || r + n >= len)
			return 0;
		n += r;
	}
	if(uri->fragment && *uri->fragment)
	{
		r = snprintf(buf + n, len - n, "#%s", uri->fragment);
		if(r < 0 || r + n >= len)
			return 0;
		n += r;
	}
	return n;
}

int uri_userpass(struct uri_t * uri, char * user, int ul, char * pass, int pl)
{
	char * sep;

	if(!uri->userpass)
	{
		user[0] = '\0';
		pass[0] = '\0';
	}
	else
	{
		sep = strchr(uri->userpass, ':');
		if(sep)
		{
			snprintf(user, ul, "%.*s", (int)(sep - uri->userpass), uri->userpass);
			snprintf(pass, pl, "%s", sep + 1);
		}
		else
		{
			snprintf(user, ul, "%s", uri->userpass);
			pass[0] = '\0';
		}
	}
	return 1;
}

int uri_query(const char * query, struct uri_query_t ** info)
{
	struct uri_query_t items[64], * pp;

	if(query && info)
	{
		const char * end = query + strlen(query);
		const char * p;
		int capacity = 0;
		int count = 0;
		*info = NULL;
		for(p = query; p && (p < end); query = p + 1)
		{
			p = strpbrk(query, "&=");
			if(!p || p > end)
				break;
			if(p == query)
			{
				if('&' == *p)
					continue;
				else
				{
					uri_query_free(info);
					return 0;
				}
			}
			if(count < ARRAY_SIZE(items))
				pp = &items[count++];
			else
			{
				if(count >= capacity)
				{
					capacity = count + 64;
					pp = (struct uri_query_t *)realloc(*info, capacity * sizeof(struct uri_query_t));
					if(!pp)
						return 0;
					*info = pp;
				}
				pp = &(*info)[count++];
			}
			pp->key = query;
			pp->nkey = (int)(p - query);
			if(*p == '=')
			{
				pp->value = p + 1;
				p = strchr(pp->value, '&');
				if(NULL == p)
					p = end;
				pp->nvalue = (int)(p - pp->value);
			}
			else
			{
				pp->value = NULL;
				pp->nvalue = 0;
			}
		}
		if((count <= ARRAY_SIZE(items)) && (count > 0))
		{
			*info = (struct uri_query_t *)malloc(count * sizeof(struct uri_query_t));
			if(!*info)
				return 0;
			memcpy(*info, items, count * sizeof(struct uri_query_t));
		}
		else if(count > ARRAY_SIZE(items))
			memcpy(*info, items, ARRAY_SIZE(items) * sizeof(struct uri_query_t));
		return count;
	}
	return 0;
}

void uri_query_free(struct uri_query_t ** info)
{
	if(info && *info)
	{
		free(*info);
		*info = NULL;
	}
}
