/*
 * xboot/lib/string.c
 */

#include <configs.h>
#include <default.h>
#include <types.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>

/**
 * strnicmp - Case insensitive, length-limited string comparison
 * @s1: One string
 * @s2: The other string
 * @len: the maximum number of characters to compare
 */
x_s32 strnicmp(const x_s8 *s1, const x_s8 *s2, x_s32 len)
{
	x_u8 c1=0,c2=0;

	if (len)
	{
		do {
			c1 = *s1;
			c2 = *s2;
			s1++;
			s2++;
			if (!c1)
				break;
			if (!c2)
				break;
			if (c1 == c2)
				continue;
			c1 = tolower(c1);
			c2 = tolower(c2);
			if (c1 != c2)
				break;
		} while (--len);
	}
	return (x_s32)c1 - (x_s32)c2;
}

/**
 * strcpy - Copy a %NUL terminated string
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 */
x_s8 * strcpy(x_s8 *dest, const x_s8 *src)
{
	x_s8 *tmp = dest;

	while ((*dest++ = *src++) != '\0');
	return tmp;
}

/**
 * strdup - returns a pointer to a new string
 * @s: Where to copy the string from
 */
x_s8 * strdup(const x_s8 * src)
{
	x_s8 * p;

	if(!src)
		return NULL;

	p = malloc(strlen(src) + 1);
	if(p)
		return(strcpy(p, src));

	return NULL;
}

/**
 * strncpy - Copy a length-limited, %NUL-terminated string
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @count: The maximum number of bytes to copy
 *
 * The result is not %NUL-terminated if the source exceeds
 * @count bytes.
 *
 * In the case where the length of @src is less than  that  of
 * count, the remainder of @dest will be padded with %NUL.
 *
 */
x_s8 * strncpy(x_s8 *dest, const x_s8 *src, x_s32 count)
{
	x_s8 *tmp = dest;

	while (count)
	{
		if ((*tmp = *src) != 0)
			src++;
		tmp++;
		count--;
	}
	return dest;
}

/**
 * strlcpy - Copy a %NUL terminated string into a sized buffer
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @size: size of destination buffer
 *
 * Compatible with *BSD: the result is always a valid
 * NUL-terminated string that fits in the buffer (unless,
 * of course, the buffer size is zero). It does not pad
 * out the result like strncpy() does.
 */
x_s32 strlcpy(x_s8 *dest, const x_s8 *src, x_s32 size)
{
	x_s32 len;
	x_s32 ret = strlen(src);

	if (size)
	{
		len = (ret >= size) ? size - 1 : ret;
		memcpy(dest, src, len);
		dest[len] = '\0';
	}
	return ret;
}

/**
 * strcat - Append one %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 */
x_s8 * strcat(x_s8 *dest, const x_s8 *src)
{
	x_s8 *tmp = dest;

	while (*dest)
		dest++;
	while ((*dest++ = *src++) != '\0');
	return tmp;
}

/**
 * strncat - Append a length-limited, %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 * @count: The maximum numbers of bytes to copy
 *
 * Note that in contrast to strncpy(), strncat() ensures the result is
 * terminated.
 */
x_s8 * strncat(x_s8 *dest, const x_s8 *src, x_s32 count)
{
	x_s8 *tmp = dest;

	if (count)
	{
		while (*dest)
			dest++;
		while ((*dest++ = *src++) != 0)
		{
			if (--count == 0)
			{
				*dest = '\0';
				break;
			}
		}
	}
	return tmp;
}

/**
 * strlcat - Append a length-limited, %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 * @count: The size of the destination buffer.
 */
x_s32 strlcat(x_s8 *dest, const x_s8 *src, x_s32 count)
{
	x_s32 dsize = strlen(dest);
	x_s32 len = strlen(src);
	x_s32 res = dsize + len;

	dest += dsize;
	count -= dsize;
	if (len >= count)
		len = count-1;
	memcpy(dest, src, len);
	dest[len] = 0;
	return res;
}

/**
 * strcmp - Compare two strings
 * @cs: One string
 * @ct: Another string
 */
x_s32 strcmp(const x_s8 *cs, const x_s8 *ct)
{
	x_s32 __res;

	while (1)
	{
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
	}
	return __res;
}

/**
 * strncmp - Compare two length-limited strings
 * @cs: One string
 * @ct: Another string
 * @count: The maximum number of bytes to compare
 */
x_s32 strncmp(const x_s8 *cs, const x_s8 *ct, x_s32 count)
{
	x_s32 __res = 0;

	while (count)
	{
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
		count--;
	}
	return __res;
}

/**
 * strchr - Find the first occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 */
x_s8 * strchr(const x_s8 *s, x_s32 c)
{
	for (; *s != (x_s8)c; ++s)
		if (*s == '\0')
			return NULL;
	return (x_s8 *)s;
}

/**
 * strrchr - Find the last occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 */
x_s8 * strrchr(const x_s8 *s, x_s32 c)
{
       const x_s8 *p = s + strlen(s);
       do {
           if (*p == (x_s8)c)
               return (x_s8 *)p;
       } while (--p >= s);
       return NULL;
}

/**
 * strnchr - Find a character in a length limited string
 * @s: The string to be searched
 * @count: The number of characters to be searched
 * @c: The character to search for
 */
x_s8 * strnchr(const x_s8 *s, x_s32 count, x_s32 c)
{
	for (; count-- && *s != '\0'; ++s)
		if (*s == (x_s8)c)
			return (x_s8 *)s;
	return NULL;
}

/**
 * strstrip - Removes leading and trailing whitespace from @s.
 * @s: The string to be stripped.
 *
 * Note that the first trailing whitespace is replaced with a %NUL-terminator
 * in the given string @s. Returns a pointer to the first non-whitespace
 * character in @s.
 */
x_s8 * strstrip(x_s8 *s)
{
	x_s32 size;
	x_s8 *end;

	size = strlen(s);

	if (!size)
		return s;

	end = s + size - 1;
	while (end >= s && isspace(*end))
		end--;
	*(end + 1) = '\0';

	while (*s && isspace(*s))
		s++;

	return s;
}

/**
 * strlen - Find the length of a string
 * @s: The string to be sized
 */
x_s32 strlen(const x_s8 *s)
{
	const x_s8 *sc;

	for (sc = s; *sc != '\0'; ++sc);
	return sc - s;
}

/**
 * strnlen - Find the length of a length-limited string
 * @s: The string to be sized
 * @count: The maximum number of bytes to search
 */
x_s32 strnlen(const x_s8 *s, x_s32 count)
{
	const x_s8 *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc);
	return sc - s;
}

/**
 * strspn - Calculate the length of the initial substring of @s which only contain letters in @accept
 * @s: The string to be searched
 * @accept: The string to search for
 */
x_s32 strspn(const x_s8 *s, const x_s8 *accept)
{
	const x_s8 *p;
	const x_s8 *a;
	x_s32 count = 0;

	for (p = s; *p != '\0'; ++p)
	{
		for (a = accept; *a != '\0'; ++a)
		{
			if (*p == *a)
				break;
		}
		if (*a == '\0')
			return count;
		++count;
	}
	return count;
}

/**
 * strcspn - Calculate the length of the initial substring of @s which does not contain letters in @reject
 * @s: The string to be searched
 * @reject: The string to avoid
 */
x_s32 strcspn(const x_s8 *s, const x_s8 *reject)
{
	const x_s8 *p;
	const x_s8 *r;
	x_s32 count = 0;

	for (p = s; *p != '\0'; ++p)
	{
		for (r = reject; *r != '\0'; ++r)
		{
			if (*p == *r)
				return count;
		}
		++count;
	}
	return count;
}

/**
 * strpbrk - Find the first occurrence of a set of characters
 * @cs: The string to be searched
 * @ct: The characters to search for
 */
x_s8 * strpbrk(const x_s8 *cs, const x_s8 *ct)
{
	const x_s8 *sc1, *sc2;

	for (sc1 = cs; *sc1 != '\0'; ++sc1)
	{
		for (sc2 = ct; *sc2 != '\0'; ++sc2)
		{
			if (*sc1 == *sc2)
				return (x_s8 *)sc1;
		}
	}
	return NULL;
}

/**
 * strsep - Split a string into tokens
 * @s: The string to be searched
 * @ct: The characters to search for
 *
 * strsep() updates @s to point after the token, ready for the next call.
 *
 * It returns empty tokens, too, behaving exactly like the libc function
 * of that name. In fact, it was stolen from glibc2 and de-fancy-fied.
 * Same semantics, slimmer shape. ;)
 */
x_s8 * strsep(x_s8 **s, const x_s8 *ct)
{
	x_s8 *sbegin = *s;
	x_s8 *end;

	if (sbegin == NULL)
		return NULL;

	end = strpbrk(sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;
	return sbegin;
}

/**
 * memscan - Find a character in an area of memory.
 * @addr: The memory area
 * @c: The byte to search for
 * @size: The size of the area.
 *
 * returns the address of the first occurrence of @c, or 1 byte past
 * the area if @c is not found
 */
void * memscan(void *addr, x_s32 c, x_s32 size)
{
	x_u8 *p = addr;

	while (size)
	{
		if (*p == c)
			return (void *)p;
		p++;
		size--;
	}
  	return (void *)p;
}

/**
 * strstr - Find the first substring in a %NUL terminated string
 * @s1: The string to be searched
 * @s2: The string to search for
 */
x_s8 * strstr(const x_s8 *s1, const x_s8 *s2)
{
	x_s32 l1, l2;

	l2 = strlen(s2);
	if (!l2)
		return (x_s8 *)s1;
	l1 = strlen(s1);
	while (l1 >= l2)
	{
		l1--;
		if (!memcmp(s1, s2, l2))
			return (x_s8 *)s1;
		s1++;
	}
	return NULL;
}

/**
 * memchr - Find a character in an area of memory.
 * @s: The memory area
 * @c: The byte to search for
 * @n: The size of the area.
 *
 * returns the address of the first occurrence of @c, or %NULL
 * if @c is not found
 */
void * memchr(const void *s, x_s32 c, x_s32 n)
{
	const x_u8 *p = s;
	while (n-- != 0)
	{
        if ((x_u8)c == *p++)
        {
			return (void *)(p - 1);
		}
	}
	return NULL;
}

/**
 * memset - Fill a region of memory with the given value
 * @s: Pointer to the start of the area.
 * @c: The byte to fill the area with
 * @count: The size of the area.
 *
 * This needs to be optimized.
 */
void * __attribute__((weak)) memset(void *s, x_s32 c, x_s32 count)
{
	x_s8 *xs = s;

	while (count--)
		*xs++ = c;
	return s;
}

/**
 * memcpy - Copy one area of memory to another
 * @dest: Where to copy to
 * @src: Where to copy from
 * @count: The size of the area.
 *
 * This needs to be optimized.
 */
void * __attribute__((weak)) memcpy(void *dest, const void *src, x_s32 count)
{
	x_s8 *tmp = dest;
	const x_s8 *s = src;

	while (count--)
		*tmp++ = *s++;
	return dest;
}

/**
 * memmove - Copy one area of memory to another
 * @dest: Where to copy to
 * @src: Where to copy from
 * @count: The size of the area.
 *
 * This needs to be optimized.
 */
void * __attribute__((weak)) memmove(void *dest, const void *src, x_s32 count)
{
	x_s8 *tmp;
	const x_s8 *s;

	if (dest <= src)
	{
		tmp = dest;
		s = src;
		while (count--)
			*tmp++ = *s++;
	}
	else
	{
		tmp = dest;
		tmp += count;
		s = src;
		s += count;
		while (count--)
			*--tmp = *--s;
	}
	return dest;
}

/**
 * memcmp - Compare two areas of memory
 * @cs: One area of memory
 * @ct: Another area of memory
 * @count: The size of the area.
 *
 * This needs to be optimized.
 */
x_s32 __attribute__((weak)) memcmp(const void *cs, const void *ct, x_s32 count)
{
	const x_u8 *su1, *su2;
	x_s32 res = 0;

	for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
		if ((res = *su1 - *su2) != 0)
			break;
	return res;
}
