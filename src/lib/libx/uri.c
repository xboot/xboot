/*
 * libx/uri.c
 */

#include <types.h>
#include <ctype.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <uri.h>

static const char * find_string(const char * s, size_t len, const char * str)
{
	const char * end = s + len;
	const char * p = s;
	size_t n = strlen(str);
	int i;

	while(p < end)
	{
		for(i = 0; i < n; i++)
		{
			if(*p == str[i])
				return p;
		}
		p++;
	}
	return NULL;
}

static const char * find_string_reverse(const char * s, size_t len, const char * str)
{
	const char * end = s + len;
	const char * p = end - 1;
	size_t n = strlen(str);
	int i;

	while(p >= s)
	{
		for(i = 0; i < n; i++)
		{
			if(*p == str[i])
				return p;
		}
		p--;
	}
	return NULL;
}

static const char * lookup_scheme(const char * s)
{
	const char * p = s;
	char c;

	if(strlen(s) == 0)
		return NULL;
	if(!isalpha(*p))
		return NULL;
	p++;
	while(*p != '\0')
	{
		c = *p;
		if(c == ':')
			return p;
		if(!isalpha(c) && !isdigit(c) && (c != '+') && (c != '-') && (c != '.'))
			return NULL;
		p++;
	}
	return NULL;
}

static int parse_user_password(const char * s, size_t len, struct uri_t * uri)
{
	const char * end = s + len;
	const char * found;

	found = strnstr(s, ":", len);
	if(found)
	{
		uri->user = strndup(s, found - s);
		if(uri->user == NULL)
			return 0;
		uri->pass = strndup(found + 1, end - found - 1);
		if(uri->pass == NULL)
			return 0;
	}
	else
	{
		uri->user = strndup(s, len);
		if(uri->user == NULL)
			return 0;
	}
	return 1;
}

static int parse_authority(const char * s, size_t len, struct uri_t * uri)
{
	const char * end = s + len;
	const char * p, * found, * host_start, * host_end;
	int port;

	uri->port = 0;
	if(len == 0)
		return 1;
	found = strnstr(s, "@", len);
	if(found)
	{
		if(!parse_user_password(s, found - s, uri))
			return 0;
		host_start = found + 1;
	}
	else
		host_start = s;
	if(*host_start == '[')
	{
		if(find_string(host_start + 1, end - host_start - 1, "["))
			return 0;
		host_end = find_string(host_start + 1, end - host_start - 1, "]");
		if(!host_end)
			return 0;
		if(host_end + 1 != end && host_end[1] != ':')
			return 0;
		host_end++;
	}
	else
	{
		host_end = find_string_reverse(host_start, end - host_start, ":");
		if(host_end == NULL)
			host_end = end;
		if(find_string(host_start, host_end - host_start, "[]"))
			return 0;
	}
	if(find_string(host_start, host_end - host_start, " "))
		return 0;
	if(host_end == end)
	{
		if(host_start == end)
			return 0;
		uri->host = strndup(host_start, end - host_start);
		if(uri->host == NULL)
			return 0;
		return 1;
	}
	if(host_start == host_end)
		return 0;
	if(host_end + 1 < end)
	{
		p = host_end + 1;
		port = 0;
		while(p < end)
		{
			if(*p < '0' || *p > '9')
				return 0;
			port = port * 10 + *p - '0';
			if(port > 65535)
				return 0;
			p++;
		}
	}
	else
		port = 0;
	uri->host = strndup(host_start, (size_t)(host_end - host_start));
	if(uri->host == NULL)
		return 0;
	uri->port = port;
	return 1;
}

struct uri_t * uri_alloc(const char * s)
{
	struct uri_t * uri;
	const char * p;
	const char * end;
	const char * found;
	size_t len;

	if(!s)
		return NULL;
	end = s + strlen(s);
	for(p = s; p < end; p++)
	{
		if(iscntrl(*p))
			return NULL;
	}
	uri = malloc(sizeof(struct uri_t));
	if(!uri)
		return NULL;
	memset(uri, 0, sizeof(struct uri_t));
	uri->port = 0;
	p = s;
	found = lookup_scheme(p);
	if(found)
	{
		uri->scheme = strndup(s, (size_t)(found - p));
		if(uri->scheme == NULL)
			goto error;
		p = found + 1;
		if(p >= end)
			return uri;
	}
	if((strlen(p) >= 2) && (p[0] == '/') && (p[1] == '/'))
	{
		p = p + 2;
		found = find_string(p, strlen(p), "/?#");
		if(found == NULL)
			len = strlen(p);
		else
			len = (size_t)(found - p);
		if(!parse_authority(p, len, uri))
			goto error;
		if(!found)
			return uri;
		p = found;
	}
	if((*p != '?') && (*p != '#'))
	{
		found = find_string(p, strlen(p), "?#");
		if(found == NULL)
		{
			uri->path = strdup(p);
			if(uri->path == NULL)
				goto error;
		}
		else
		{
			if(found != p)
			{
				uri->path = strndup(p, (size_t)(found - p));
				if(uri->path == NULL)
					goto error;
			}
		}
		if(!found)
			return uri;
		p = found;
	}
	if(*p == '?')
	{
		p = p + 1;
		found = find_string(p, strlen(p), "#");
		if(found == NULL)
			uri->query = strdup(p);
		else
			uri->query = strndup(p, (size_t)(found - p));
		if(uri->query == NULL)
			goto error;
		if(!found)
			return uri;
		p = found;
	}
	p = p + 1;
	uri->fragment = strdup(p);
	if(uri->fragment == NULL)
		goto error;
	return uri;
error:
	free(uri);
	return NULL;
}

void uri_free(struct uri_t * uri)
{
	if(uri)
	{
		if(uri->scheme)
			free(uri->scheme);
		if(uri->user)
			free(uri->user);
		if(uri->pass)
			free(uri->pass);
		if(uri->host)
			free(uri->host);
		if(uri->path)
			free(uri->path);
		if(uri->query)
			free(uri->query);
		if(uri->fragment)
			free(uri->fragment);
		free(uri);
	}
}
