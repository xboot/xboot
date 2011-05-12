/*
 * xboot/lib/vsprintf.c
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <stddef.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <div64.h>
#include <vsprintf.h>

/*
 * simple_strtou32 - convert a string to x_u32
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 */
x_u32 simple_strtou32(const x_s8 *cp, x_s8 **endp, x_u32 base)
{
	x_u32 result = 0,value;

	if (!base)
	{
		base = 10;
		if (*cp == '0')
		{
			base = 8;
			cp++;
			if ((toupper(*cp) == 'X') && isxdigit(cp[1]))
			{
				cp++;
				base = 16;
			}
		}
	}
	else if (base == 16)
	{
		if (cp[0] == '0' && toupper(cp[1]) == 'X')
			cp += 2;
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : toupper(*cp)-'A'+10) < base)
	{
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (x_s8 *)cp;
	return result;
}

/*
 * simple_strtos32 - convert a string to x_s32
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 */
x_s32 simple_strtos32(const x_s8 *cp, x_s8 **endp, x_u32 base)
{
	if(*cp=='-')
		return -simple_strtou32(cp+1,endp,base);
	return simple_strtou32(cp,endp,base);
}

/*
 * simple_strtou64 - convert a string to x_u64
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 */
x_u64 simple_strtou64(const x_s8 *cp, x_s8 **endp, x_u32 base)
{
	x_u64 result = 0,value;

	if (!base)
	{
		base = 10;
		if (*cp == '0')
		{
			base = 8;
			cp++;
			if ((toupper(*cp) == 'X') && isxdigit(cp[1]))
			{
				cp++;
				base = 16;
			}
		}
	}
	else if (base == 16)
	{
		if (cp[0] == '0' && toupper(cp[1]) == 'X')
			cp += 2;
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp) ? toupper(*cp) : *cp)-'A'+10) < base)
	{
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (x_s8 *)cp;
	return result;
}

/*
 * simple_strtos64 - convert a string to x_s64
 * @cp: The start of the string
 * @endp: A pointer to the end of the parsed string will be placed here
 * @base: The number base to use
 */
x_s64 simple_strtos64(const x_s8 *cp, x_s8 **endp, x_u32 base)
{
	if(*cp=='-')
		return -simple_strtou64(cp+1,endp,base);
	return simple_strtou64(cp,endp,base);
}

/*
 * skip_atos32 - convert a string to x_s32
 */
static x_s32 skip_atos32(const x_s8 **s)
{
	x_s32 i=0;
	while (isdigit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */

static x_s8 * number(x_s8 * buf, x_s8 * end, x_u64 num, x_u32 base, x_s32 size, x_s32 precision, x_s32 type)
{
	x_s8 c,sign,tmp[66];
	const x_s8 *digits;
	static const x_s8 small_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	static const x_s8 large_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	x_s32 i;

	digits = (type & LARGE) ? large_digits : small_digits;
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return NULL;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN)
	{
		if ((x_s64) num < 0)
		{
			sign = '-';
			num = - (x_s64) num;
			size--;
		}
		else if (type & PLUS)
		{
			sign = '+';
			size--;
		}
		else if (type & SPACE)
		{
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL)
	{
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0)
	{
		tmp[i++]='0';
	}
	else
	{
		while(num != 0)
		{
			tmp[i++] = digits[div64_64(&num, base)];
		}
	}
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
	{
		while(size-->0)
		{
			if (buf < end)
				*buf = ' ';
			++buf;
		}
	}
	if (sign)
	{
		if (buf < end)
			*buf = sign;
		++buf;
	}
	if (type & SPECIAL)
	{
		if (base==8) {
			if (buf < end)
				*buf = '0';
			++buf;
		}
		else if (base==16)
		{
			if (buf < end)
				*buf = '0';
			++buf;
			if (buf < end)
				*buf = digits[33];
			++buf;
		}
	}
	if (!(type & LEFT))
	{
		while (size-- > 0)
		{
			if (buf < end)
				*buf = c;
			++buf;
		}
	}
	while (i < precision--)
	{
		if (buf < end)
			*buf = '0';
		++buf;
	}
	while (i-- > 0)
	{
		if (buf < end)
			*buf = tmp[i];
		++buf;
	}
	while (size-- > 0)
	{
		if (buf < end)
			*buf = ' ';
		++buf;
	}
	return buf;
}

/*
 * vsnprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @args: Arguments for the format string
 *
 * The return value is the number of characters which would
 * be generated for the given input, excluding the trailing
 * '\0', as per ISO C99. If you want to have the exact
 * number of characters written into @buf as return value
 * (not including the trailing '\0'), use vscnprintf(). If the
 * return is greater than or equal to @size, the resulting
 * string is truncated.
 *
 * Call this function if you are already dealing with a va_list.
 * You probably want snprintf() instead.
 */
x_s32 vsnprintf(x_s8 *buf, x_s32 size, const x_s8 *fmt, va_list args)
{
	x_s32 len;
	x_u64 num;
	x_s32 i, base;
	x_s8 *str, *end, c;
	const x_s8 *s;

	x_s32 flags;		/* flags to number() */

	x_s32 field_width;	/* width of output field */
	x_s32 precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	x_s32 qualifier;		/* 'h', 'l', or 'L' for integer fields */
				/* 'z' support added 23/7/1999 S.H.    */
				/* 'z' changed to 'Z' --davidm 1/25/99 */
				/* 't' added for ptrdiff_t */

	if (size < 0)
		return 0;

	str = buf;
	end = buf + size;

	/* Make sure end is always >= buf */
	if (end < buf)
	{
		end = ((void *)-1);
		size = end - buf;
	}

	for (; *fmt ; ++fmt)
	{
		if (*fmt != '%')
		{
			if (str < end)
				*str = *fmt;
			++str;
			continue;
		}

		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt)
			{
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
			}

		/* get field width */
		field_width = -1;
		if (isdigit(*fmt))
			field_width = skip_atos32(&fmt);
		else if (*fmt == '*')
		{
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, x_s32);
			if (field_width < 0)
			{
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.')
		{
			++fmt;
			if (isdigit(*fmt))
				precision = skip_atos32(&fmt);
			else if (*fmt == '*')
			{
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, x_s32);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' ||
		    *fmt =='Z' || *fmt == 'z' || *fmt == 't') {
			qualifier = *fmt;
			++fmt;
			if (qualifier == 'l' && *fmt == 'l')
			{
				qualifier = 'L';
				++fmt;
			}
		}

		/* default base */
		base = 10;

		switch (*fmt)
		{
			case 'c':
				if (!(flags & LEFT))
				{
					while (--field_width > 0)
					{
						if (str < end)
							*str = ' ';
						++str;
					}
				}
				c = (x_u8) va_arg(args, x_s32);
				if (str < end)
					*str = c;
				++str;
				while (--field_width > 0)
				{
					if (str < end)
						*str = ' ';
					++str;
				}
				continue;

			case 's':
				s = va_arg(args, x_s8 *);

				len = strnlen(s, precision);

				if (!(flags & LEFT))
				{
					while (len < field_width--)
					{
						if (str < end)
							*str = ' ';
						++str;
					}
				}
				for (i = 0; i < len; ++i)
				{
					if (str < end)
						*str = *s;
					++str; ++s;
				}
				while (len < field_width--)
				{
					if (str < end)
						*str = ' ';
					++str;
				}
				continue;

			case 'p':
				if (field_width == -1)
				{
					field_width = 2*sizeof(void *);
					flags |= ZEROPAD;
				}
				str = number(str, end,
						(x_u32) va_arg(args, void *),
						16, field_width, precision, flags);
				continue;


			case 'n':
				if (qualifier == 'l')
				{
					x_s32 * ip = va_arg(args, x_s32 *);
					*ip = (str - buf);
				}
				else if (qualifier == 'Z' || qualifier == 'z')
				{
					x_s32 * ip = va_arg(args, x_s32 *);
					*ip = (str - buf);
				}
				else
				{
					x_s32 * ip = va_arg(args, x_s32 *);
					*ip = (str - buf);
				}
				continue;

			case '%':
				if (str < end)
					*str = '%';
				++str;
				continue;

				/* integer number formats - set up the flags and "break" */
			case 'o':
				base = 8;
				break;

			case 'X':
				flags |= LARGE;
			case 'x':
				base = 16;
				break;

			case 'd':
			case 'i':
				flags |= SIGN;
			case 'u':
				break;

			default:
				if (str < end)
					*str = '%';
				++str;
				if (*fmt)
				{
					if (str < end)
						*str = *fmt;
					++str;
				}
				else
				{
					--fmt;
				}
				continue;
		}
		if (qualifier == 'L')
			num = va_arg(args, x_s64);
		else if (qualifier == 'l')
		{
			num = va_arg(args, x_u32);
			if (flags & SIGN)
				num = (x_s32) num;
		}
		else if (qualifier == 'Z' || qualifier == 'z')
		{
			num = va_arg(args, x_s32);
		}
		else if (qualifier == 't')
		{
			num = va_arg(args, x_s32);
		}
		else if (qualifier == 'h')
		{
			num = (x_u16) va_arg(args, x_s32);
			if (flags & SIGN)
				num = (x_s16) num;
		}
		else
		{
			num = va_arg(args, x_u32);
			if (flags & SIGN)
				num = (x_s32) num;
		}
		str = number(str, end, num, base, field_width, precision, flags);
	}
	if (size > 0)
	{
		if (str < end)
			*str = '\0';
		else
			end[-1] = '\0';
	}
	/* the trailing null byte doesn't count towards the total */
	return str-buf;
}

/**
 * vsscanf - Unformat a buffer into a list of arguments
 * @buf:	input buffer
 * @fmt:	format of buffer
 * @args:	arguments
 */
x_s32 vsscanf(const x_s8 * buf, const x_s8 * fmt, va_list args)
{
	const x_s8 *str = buf;
	x_s8 *next;
	x_s8 digit;
	x_s32 num = 0;
	x_s32 qualifier;
	x_u8 base;
	x_s32 field_width;
	x_s32 is_sign = 0;

	while(*fmt && *str)
	{
		/* skip any white space in format */
		/* white space in format matchs any amount of
		 * white space, including none, in the input.
		 */
		if (isspace(*fmt))
		{
			while (isspace(*fmt))
				++fmt;
			while (isspace(*str))
				++str;
		}

		/* anything that is not a conversion must match exactly */
		if (*fmt != '%' && *fmt)
		{
			if (*fmt++ != *str++)
				break;
			continue;
		}

		if (!*fmt)
			break;
		++fmt;

		/* skip this conversion.
		 * advance both strings to next white space
		 */
		if (*fmt == '*')
		{
			while (!isspace(*fmt) && *fmt)
				fmt++;
			while (!isspace(*str) && *str)
				str++;
			continue;
		}

		/* get field width */
		field_width = -1;
		if (isdigit(*fmt))
			field_width = skip_atos32(&fmt);

		/* get conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' ||
		    *fmt == 'Z' || *fmt == 'z') {
			qualifier = *fmt++;
			if (qualifier == *fmt)
			{
				if (qualifier == 'h')
				{
					qualifier = 'H';
					fmt++;
				}
				else if (qualifier == 'l')
				{
					qualifier = 'L';
					fmt++;
				}
			}
		}
		base = 10;
		is_sign = 0;

		if (!*fmt || !*str)
			break;

		switch(*fmt++)
		{
		case 'c':
		{
			x_s8 *s = (x_s8 *) va_arg(args,x_s8*);
			if (field_width == -1)
				field_width = 1;
			do {
				*s++ = *str++;
			} while (--field_width > 0 && *str);
			num++;
		}
		continue;
		case 's':
		{
			x_s8 *s = (x_s8 *) va_arg(args, x_s8 *);
			if(field_width == -1)
				field_width = ((x_s32)(~0U>>1));
			/* first, skip leading white space in buffer */
			while (isspace(*str))
				str++;

			/* now copy until next white space */
			while (*str && !isspace(*str) && field_width--)
			{
				*s++ = *str++;
			}
			*s = '\0';
			num++;
		}
		continue;
		case 'n':
			/* return number of characters read so far */
		{
			x_s32 *i = (x_s32 *)va_arg(args,x_s32*);
			*i = str - buf;
		}
		continue;
		case 'o':
			base = 8;
			break;
		case 'x':
		case 'X':
			base = 16;
			break;
		case 'i':
			base = 0;
		case 'd':
			is_sign = 1;
		case 'u':
			break;
		case '%':
			/* looking for '%' in str */
			if (*str++ != '%')
				return num;
			continue;
		default:
			/* invalid format; stop here */
			return num;
		}

		/* have some sort of integer conversion.
		 * first, skip white space in buffer.
		 */
		while (isspace(*str))
			str++;

		digit = *str;
		if (is_sign && digit == '-')
			digit = *(str + 1);

		if (!digit
                    || (base == 16 && !isxdigit(digit))
                    || (base == 10 && !isdigit(digit))
                    || (base == 8 && (!isdigit(digit) || digit > '7'))
                    || (base == 0 && !isdigit(digit)))
				break;

		switch(qualifier)
		{
		case 'H':	/* that's 'hh' in format */
			if (is_sign) {
				x_s8 *s = (x_s8 *) va_arg(args,x_s8 *);
				*s = (x_s8) simple_strtos32(str,&next,base);
			} else {
				x_u8 *s = (x_u8 *) va_arg(args, x_u8 *);
				*s = (x_u8) simple_strtou32(str, &next, base);
			}
			break;
		case 'h':
			if (is_sign)
			{
				x_s16 *s = (x_s16 *) va_arg(args,x_s16 *);
				*s = (x_s16) simple_strtos32(str,&next,base);
			}
			else
			{
				x_u16 *s = (x_u16 *) va_arg(args, x_u16 *);
				*s = (x_u16) simple_strtou32(str, &next, base);
			}
			break;
		case 'l':
			if (is_sign)
			{
				x_s32 *l = (x_s32 *) va_arg(args,x_s32 *);
				*l = simple_strtos32(str,&next,base);
			} else {
				x_u32 *l = (x_u32*) va_arg(args,x_u32*);
				*l = simple_strtou32(str,&next,base);
			}
			break;
		case 'L':
			if (is_sign)
			{
				x_s64 *l = (x_s64*) va_arg(args,x_s64 *);
				*l = simple_strtos64(str,&next,base);
			}
			else
			{
				x_u64 *l = (x_u64*) va_arg(args,x_u64*);
				*l = simple_strtou64(str,&next,base);
			}
			break;
		case 'Z':
		case 'z':
		{
			x_u32 *s = (x_u32*) va_arg(args,x_u32*);
			*s = (x_u32) simple_strtou32(str,&next,base);
		}
		break;
		default:
			if (is_sign)
			{
				x_s32 *i = (x_s32 *) va_arg(args, x_s32*);
				*i = (x_s32) simple_strtos32(str,&next,base);
			}
			else
			{
				x_u32 *i = (x_u32*) va_arg(args, x_u32*);
				*i = (x_u32) simple_strtou32(str,&next,base);
			}
			break;
		}
		num++;

		if (!next)
			break;
		str = next;
	}
	return num;
}

/*
 * sprintf - Format a string and place it in a buffer
 * @buf:	The buffer to place the result into
 * @fmt:	The format string to use
 * @...:	Arguments for the format string
 */
int sprintf(char * buf, const char * fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf((x_s8 *)buf,(x_s32)(0xFFFF>>1),(x_s8 *)fmt,args);
	va_end(args);
	return i;
}

/*
 * sprintf - Format a string and place it in a buffer
 * @buf:	The buffer to place the result into
 * @size: 	The size of the buffer, including the trailing null space
 * @fmt:	The format string to use
 * @...:	Arguments for the format string
 */
int snprintf(char * buf, size_t size, const char * fmt, ...)
{
	va_list args;
	x_s32 i;

	va_start(args, fmt);
	i = (x_s32)vsnprintf((x_s8 *)buf, size, (x_s8 *)fmt, args);
	va_end(args);
	return i;
}

/*
 * sscanf - Unformat a buffer into a list of arguments
 * @buf:	input buffer
 * @fmt:	formatting of buffer
 * @...:	resulting arguments
 */
x_s32 sscanf(const x_s8 * buf, const x_s8 * fmt, ...)
{
	va_list args;
	x_s32 i;

	va_start(args,fmt);
	i = vsscanf((x_s8 *)buf,(x_s8 *)fmt,args);
	va_end(args);
	return i;
}

/*
 * ssize - format size to string
 */
x_s32 ssize(char * buf, x_u64 size)
{
	const char * unit[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
	x_s32 count = 0;

	while( ((size >> 10) > 0) && (count < 8) )
	{
		size = size >> 10;
		count++;
	}

	return( sprintf(buf, (const char *)"%lu%s", (x_u32)size, unit[count]) );
}
