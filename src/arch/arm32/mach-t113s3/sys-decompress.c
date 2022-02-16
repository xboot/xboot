/*
 * sys-decompress.c
 *
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>

#define MINMATCH 			(4)
#define WILDCOPYLENGTH		(8)
#define LASTLITERALS		(5)
#define MFLIMIT				(WILDCOPYLENGTH + MINMATCH)

#define ML_BITS				(4)
#define ML_MASK				((1 << ML_BITS) - 1)
#define RUN_BITS			(8 - ML_BITS)
#define RUN_MASK			((1 << RUN_BITS) - 1)

static unsigned int lz4_is_little_endian(void)
{
	union { uint32_t u; uint8_t c[4]; } one = { 1 };
	return one.c[0];
}

static uint16_t lz4_read16(void * mem)
{
	uint16_t val;
	memcpy(&val, mem, sizeof(val));
	return val;
}

static void lz4_write32(void * mem, uint32_t val)
{
	memcpy(mem, &val, sizeof(val));
}

static uint16_t lz4_read_le16(void * mem)
{
	uint8_t * p = (uint8_t *)mem;
	if(lz4_is_little_endian())
		return lz4_read16(mem);
	return (uint16_t)((uint16_t)p[0] + (p[1] << 8));
}

void lz4_wildcopy(void * dst, void * src, void * edst)
{
	uint8_t * d = (uint8_t *)dst;
	uint8_t * s = (uint8_t *)src;
	uint8_t * e = (uint8_t *)edst;
	do {
		memcpy(d, s, 8);
		d += 8;
		s += 8;
	} while(d < e);
}

void sys_decompress(char * src, int slen, char * dst, int dlen)
{
	uint8_t * ip = (uint8_t *) src;
	uint8_t * iend = ip + slen;
	uint8_t * op = (uint8_t *)dst;
	uint8_t * oend = op + dlen;
	uint8_t * lowprefix = (uint8_t *)dst;
	uint8_t * cpy;
	uint8_t * shortiend = iend - 14 - 2;
	uint8_t * shortoend = oend - 14 - 18;
	uint8_t * copylimit;
	uint8_t * match;
	size_t offset;
	size_t length;
	unsigned int token;
	unsigned int s;
	unsigned int inc32table[8];
	int dec64table[8];

	inc32table[0] = 0;
	inc32table[1] = 1;
	inc32table[2] = 2;
	inc32table[3] = 1;
	inc32table[4] = 0;
	inc32table[5] = 4;
	inc32table[6] = 4;
	inc32table[7] = 4;

	dec64table[0] = 0;
	dec64table[1] = 0;
	dec64table[2] = 0;
	dec64table[3] = -1;
	dec64table[4] = -4;
	dec64table[5] = 1;
	dec64table[6] = 2;
	dec64table[7] = 3;

	while(1)
	{
		token = *ip++;
		length = token >> ML_BITS;

		if((length != RUN_MASK) && likely((ip < shortiend) & (op <= shortoend)))
		{
			memcpy(op, ip, 16);
			op += length;
			ip += length;

			length = token & ML_MASK;
			offset = lz4_read_le16(ip);
			ip += 2;
			match = op - offset;

			if((length != ML_MASK) && (offset >= 8) && (match >= lowprefix))
			{
				memcpy(op + 0, match + 0, 8);
				memcpy(op + 8, match + 8, 8);
				memcpy(op + 16, match + 16, 2);
				op += length + MINMATCH;
				continue;
			}
			goto copy_match;
		}

		if(length == RUN_MASK)
		{
			if(unlikely(ip >= iend - RUN_MASK))
				return;
			do {
				s = *ip++;
				length += s;
			} while(likely(ip < iend - RUN_MASK) && (s == 255));
			if(unlikely(op + length < op))
				return;
			if(unlikely(ip + length < ip))
				return;
		}

		cpy = op + length;
		if((cpy > oend - MFLIMIT) || (ip + length > iend - (2 + 1 + LASTLITERALS)))
		{
			if((ip + length != iend) || (cpy > oend))
				return;
			memcpy(op, ip, length);
			ip += length;
			op += length;
			break;
		}
		lz4_wildcopy(op, ip, cpy);
		ip += length;
		op = cpy;

		offset = lz4_read_le16(ip);
		ip += 2;
		match = op - offset;
		length = token & ML_MASK;

copy_match:
		if((unlikely(match < lowprefix)))
			return;
		lz4_write32(op, (uint32_t)offset);

		if(length == ML_MASK)
		{
			do {
				s = *ip++;
				if((ip > iend - LASTLITERALS))
					return;
				length += s;
			} while(s == 255);
			if(unlikely(op + length < op))
				return;
		}
		length += MINMATCH;

		cpy = op + length;
		if(unlikely(offset < 8))
		{
			op[0] = match[0];
			op[1] = match[1];
			op[2] = match[2];
			op[3] = match[3];
			match += inc32table[offset];
			memcpy(op + 4, match, 4);
			match -= dec64table[offset];
		}
		else
		{
			memcpy(op, match, 8);
			match += 8;
		}
		op += 8;

		if(unlikely(cpy > oend - 12))
		{
			copylimit = oend - (WILDCOPYLENGTH - 1);
			if(cpy > oend - LASTLITERALS)
				return;
			if(op < copylimit)
			{
				lz4_wildcopy(op, match, copylimit);
				match += copylimit - op;
				op = copylimit;
			}
			while(op < cpy)
				*op++ = *match++;
		}
		else
		{
			memcpy(op, match, 8);
			if(length > 16)
				lz4_wildcopy(op + 8, match + 8, cpy);
		}
		op = cpy;
	}
}
