/*
 * lib/libc/charset.c
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <malloc.h>
#include <charset.h>

/*
 * utf8_to_ucs4 - convert a UTF-8 string to UCS-4 string
 */
x_s32 utf8_to_ucs4(x_u32 * dst, x_s32 dst_size, const x_s8 * src, x_s32 src_size, const x_s8 ** src_end)
{
	x_u32 *p = dst;
	x_s32 count = 0;
	x_u32 code = 0;
	x_u32 c;

	if(src_end)
		*src_end = src;

	while(src_size && dst_size)
	{
		c = *src++;
		if(src_size != (x_s32)-1)
			src_size--;
		if(count)
		{
			if((c & 0xc0) != 0x80)
			{
				code = '?';
				/* character c may be valid, don't eat it */
				src--;
				if(src_size != (x_s32)-1)
					src_size++;
				count = 0;
			}
			else
			{
				code <<= 6;
				code |= (c & 0x3f);
				count--;
			}
		}
		else
		{
			if(c == 0)
				break;

			if((c & 0x80) == 0x00)
				code = c;
			else if((c & 0xe0) == 0xc0)
			{
				count = 1;
				code = c & 0x1f;
			}
			else if((c & 0xf0) == 0xe0)
			{
				count = 2;
				code = c & 0x0f;
			}
			else if((c & 0xf8) == 0xf0)
			{
				count = 3;
				code = c & 0x07;
			}
			else if((c & 0xfc) == 0xf8)
			{
				count = 4;
				code = c & 0x03;
			}
			else if((c & 0xfe) == 0xfc)
			{
				count = 5;
				code = c & 0x01;
			}
			else
			{
				code = '?';
				count = 0;
			}
		}

		if (count == 0)
		{
			*p++ = code;
			dst_size--;
		}
	}

	if(src_end)
		*src_end = src;

	return p - dst;
}

/*
 * utf4_to_utf8 - convert a UCS-4 to UTF-8
 */
x_s32 ucs4_to_utf8(x_u32 c, x_s8 * buf)
{
	x_s32 len = 0;

	if(buf == NULL)
		return 0;

	if(c < 0x80)
	{
		buf[len++] = c;
	}
	else if(c < 0x800)
	{
		buf[len++] = 0xc0 | ( c >> 6 );
		buf[len++] = 0x80 | ( c & 0x3f );
	}
	else if(c < 0x10000)
	{
		buf[len++] = 0xe0 | ( c >> 12 );
		buf[len++] = 0x80 | ( (c >> 6) & 0x3f );
		buf[len++] = 0x80 | ( c & 0x3f );
	}
	else if(c < 0x200000)
	{
		buf[len++] = 0xf0 | ( c >> 18 );
		buf[len++] = 0x80 | ( (c >> 12) & 0x3f );
		buf[len++] = 0x80 | ( (c >> 6) & 0x3f );
		buf[len++] = 0x80 | ( c & 0x3f );
	}
	else if(c < 0x400000)
	{
		buf[len++] = 0xf8 | ( c >> 24 );
		buf[len++] = 0x80 | ( (c >> 18) & 0x3f );
		buf[len++] = 0x80 | ( (c >> 12) & 0x3f );
		buf[len++] = 0x80 | ( (c >> 6) & 0x3f );
		buf[len++] = 0x80 | ( c & 0x3f );
	}
	else if(c < 0x80000000)
	{
		buf[len++] = 0xfc | ( c >> 30 );
		buf[len++] = 0x80 | ( (c >> 24) & 0x3f );
		buf[len++] = 0x80 | ( (c >> 18) & 0x3f );
		buf[len++] = 0x80 | ( (c >> 12) & 0x3f );
		buf[len++] = 0x80 | ( (c >> 6) & 0x3f );
		buf[len++] = 0x80 | ( c & 0x3f );
	}
	else
	{
		/* not a valid unicode character */
	}

	buf[len] = '\0';
	return len;
}

/*
 * utf8_to_utf16 - convert a UTF-8 string to UTF-16 string
 */
x_s32 utf8_to_utf16(x_u16 * dst, x_s32 dst_size, const x_s8 * src, x_s32 src_size, const x_s8 ** src_end)
{
	x_u16 *p = dst;
	x_s32 count = 0;
	x_u32 code = 0;
	x_u32 c;

	if(src_end)
		*src_end = src;

	while(src_size && dst_size)
	{
		c = *src++;
		if(src_size != (x_s32)-1)
			src_size--;
		if(count)
		{
			if ((c & 0xc0) != 0x80)
			{
				return -1;
			}
			else
			{
				code <<= 6;
				code |= (c & 0x3f);
				count--;
			}
		}
		else
		{
			if(c == 0)
				break;

			if((c & 0x80) == 0)
				code = c;
			else if((c & 0xe0) == 0xc0)
			{
				count = 1;
				code = c & 0x1f;
			}
			else if((c & 0xf0) == 0xe0)
			{
				count = 2;
				code = c & 0x0f;
			}
			else if((c & 0xf8) == 0xf0)
			{
				count = 3;
				code = c & 0x07;
			}
			else if((c & 0xfc) == 0xf8)
			{
				count = 4;
				code = c & 0x03;
			}
			else if((c & 0xfe) == 0xfc)
			{
				count = 5;
				code = c & 0x01;
			}
			else
				return -1;
		}

		if(count == 0)
		{
			if((dst_size < 2) && (code >= 0x10000))
				break;
			if(code >= 0x10000)
			{
				*p++ = (0xd800 + ((((code) - 0x10000) >> 12) & 0xfff));
				*p++ = (0xdc00 + (((code) - 0x10000) & 0xfff));
				dst_size -= 2;
			}
			else
			{
				*p++ = code;
				dst_size--;
			}
		}
    }

	if(src_end)
		*src_end = src;

	return p - dst;
}

/*
 * utf16_to_utf8 - convert a UTF-16 string to UTF-8 string
 */
x_s8 * utf16_to_utf8(x_s8 * dst, x_u16 * src, x_s32 size)
{
	x_u32 code_high = 0;
	x_u32 code;

	while(size--)
	{
		code = *src++;

		if(code_high)
		{
			if(code >= 0xDC00 && code <= 0xDFFF)
			{
				code = ((code_high - 0xD800) << 12) + (code - 0xDC00) + 0x10000;

				*dst++ = (code >> 18) | 0xF0;
				*dst++ = ((code >> 12) & 0x3F) | 0x80;
				*dst++ = ((code >> 6) & 0x3F) | 0x80;
				*dst++ = (code & 0x3F) | 0x80;
			}
			else
			{
				*dst++ = '?';
			}

			code_high = 0;
		}
		else
		{
			if(code <= 0x007F)
				*dst++ = code;
			else if(code <= 0x07FF)
			{
				*dst++ = (code >> 6) | 0xC0;
				*dst++ = (code & 0x3F) | 0x80;
			}
			else if(code >= 0xD800 && code <= 0xDBFF)
			{
				code_high = code;
				continue;
			}
			else if(code >= 0xDC00 && code <= 0xDFFF)
			{
				*dst++ = '?';
			}
			else
			{
				*dst++ = (code >> 12) | 0xE0;
				*dst++ = ((code >> 6) & 0x3F) | 0x80;
				*dst++ = (code & 0x3F) | 0x80;
			}
		}
	}

	return dst;
}

/*
 * utf4_to_utf8_alloc - convert a UCS-4 to UTF-8
 */
x_s8 * ucs4_to_utf8_alloc(x_u32 * src, x_s32 size)
{
	x_s32 remaining;
	x_u32 * ptr;
	x_u32 code;
	x_s32 cnt = 0;
	x_s8 * ret, * dest;

	remaining = size;
	ptr = src;

	while(remaining--)
	{
		code = *ptr++;

		if(code <= 0x007F)
			cnt++;
		else if(code <= 0x07FF)
			cnt += 2;
		else if((code >= 0xDC00 && code <= 0xDFFF) || (code >= 0xD800 && code <= 0xDBFF))
			cnt++;
		else
			cnt += 3;
	}
	cnt++;

	ret = malloc(cnt);
	if(!ret)
		return 0;

	dest = ret;
	remaining = size;
	ptr = src;

	while(remaining--)
	{
		code = *ptr++;

		if(code <= 0x007F)
			*dest++ = code;
		else if(code <= 0x07FF)
		{
			*dest++ = (code >> 6) | 0xC0;
			*dest++ = (code & 0x3F) | 0x80;
		}
		else if((code >= 0xDC00 && code <= 0xDFFF) || (code >= 0xD800 && code <= 0xDBFF))
		{
			*dest++ = '?';
		}
		else
		{
			*dest++ = (code >> 12) | 0xE0;
			*dest++ = ((code >> 6) & 0x3F) | 0x80;
			*dest++ = (code & 0x3F) | 0x80;
		}
	}

	*dest = 0;
	return ret;
}

/*
 * utf8_to_ucs4_alloc - convert a UTF-8 string to UCS-4 string
 */
x_s32 utf8_to_ucs4_alloc(const x_s8 * src, x_u32 ** dst, x_u32 ** pos)
{
	x_s32 len = strlen(src);

	*dst = malloc(len * sizeof(x_u32));

	if(!*dst)
		return -1;

	len = utf8_to_ucs4(*dst, len, (x_s8 *)src, -1, 0);

	*pos = *dst + len;

	return len;
}

x_bool utf8_is_valid(const x_s8 * src, x_s32 size)
{
	x_u32 code = 0;
	x_s32 count = 0;
	x_u32 c;

	while(size)
	{
		c = *src++;
		if(size != (x_s32)-1)
			size--;

		if(count)
		{
			if((c & 0xc0) != 0x80)
			{
				return FALSE;
			}
			else
			{
				code <<= 6;
				code |= (c & 0x3f);
				count--;
			}
		}
		else
		{
			if(c == 0)
				break;

			if((c & 0x80) == 0x00)
				code = c;
			else if ((c & 0xe0) == 0xc0)
			{
				count = 1;
				code = c & 0x1f;
			}
			else if ((c & 0xf0) == 0xe0)
			{
				count = 2;
				code = c & 0x0f;
			}
			else if ((c & 0xf8) == 0xf0)
			{
				count = 3;
				code = c & 0x07;
			}
			else if ((c & 0xfc) == 0xf8)
			{
				count = 4;
				code = c & 0x03;
			}
			else if ((c & 0xfe) == 0xfc)
			{
				count = 5;
				code = c & 0x01;
			}
			else
				return FALSE;
		}
	}

	return TRUE;
}

x_s32 utf8_strlen(const x_s8 * s)
{
	x_s32 i = 0, j = 0;

	while(s[i])
	{
		if((s[i] & 0xc0) != 0x80)
			j++;
		i++;
	}

	return j;
}

x_s8 * utf8_strcpy(x_s8 * dest, const x_s8 * src)
{
	x_s8 * tmp = dest;

	while((*dest++ = *src++) != '\0');

	return tmp;
}

x_s8 * utf8_strdup(const x_s8 * s)
{
	x_s8 * p;

	if(!s)
		return NULL;

	p = malloc(utf8_strlen(s) + 1);
	if(p)
		return(utf8_strcpy(p, s));

	return NULL;
}

x_s32 utf8_strcmp(const x_s8 * cs, const x_s8 * ct)
{
	x_s32 __res;

	while (1)
	{
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
	}
	return __res;
}
