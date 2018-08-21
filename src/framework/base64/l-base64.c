/*
 * framework/base64/l-base64.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
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

#include <framework/base64/l-base64.h>

static const char * mtrans = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int base64_encode(lua_State * L)
{
	const u8_t *data;
	size_t size;
	luaL_Buffer b;
	u8_t A, B, C, D;

	data = (u8_t *) luaL_checklstring(L, 1, &size);
	luaL_buffinit(L, &b);

	while(1)
	{
		switch(size)
		{
		case 0:
			luaL_pushresult(&b);
			return 1;

		case 1:
			A = (data[0] >> 2);
			B = (data[0] << 4) & 0x30;
			assert(A < 64);
			assert(B < 64);

			luaL_addchar(&b, mtrans[A]);
			luaL_addchar(&b, mtrans[B]);
			luaL_addchar(&b, '=');
			luaL_addchar(&b, '=');
			luaL_pushresult(&b);
			return 1;

		case 2:
			A = (data[0] >> 2);
			B = ((data[0] << 4) & 0x30) | ((data[1] >> 4));
			C = (data[1] << 2) & 0x3C;

			assert(A < 64);
			assert(B < 64);
			assert(C < 64);

			luaL_addchar(&b, mtrans[A]);
			luaL_addchar(&b, mtrans[B]);
			luaL_addchar(&b, mtrans[C]);
			luaL_addchar(&b, '=');
			luaL_pushresult(&b);
			return 1;

		default:
			A = (data[0] >> 2);
			B = ((data[0] << 4) & 0x30) | ((data[1] >> 4));
			C = ((data[1] << 2) & 0x3C) | ((data[2] >> 6));
			D = data[2] & 0x3F;

			assert(A < 64);
			assert(B < 64);
			assert(C < 64);
			assert(D < 64);

			luaL_addchar(&b, mtrans[A]);
			luaL_addchar(&b, mtrans[B]);
			luaL_addchar(&b, mtrans[C]);
			luaL_addchar(&b, mtrans[D]);
			size -= 3;
			data += 3;
			break;
		}
	}

	return 1;
}

static int base64_decode(lua_State * L)
{
	const char * data;
	size_t size;
	u8_t buf[4];
	luaL_Buffer b;
	size_t i;

	data = luaL_checklstring(L, 1, &size);
	if((size % 4) != 0)
		return luaL_error(L, "invalid string size for Base-64");

	luaL_buffinit(L, &b);

	while(size)
	{
		for(i = 0; i < 4; i++)
		{
			if(data[i] == '=')
				buf[i] = 0;
			else
			{
				char *p = strchr(mtrans, data[i]);
				assert(p != NULL);
				buf[i] = (p - mtrans);
				assert(buf[i] < 64);
			}
		}

		luaL_addchar(&b, (buf[0] << 2) | (buf[1] >> 4));
		luaL_addchar(&b, (buf[1] << 4) | (buf[2] >> 2));
		luaL_addchar(&b, (buf[2] << 6) | (buf[3]));

		data += 4;
		size -= 4;
	}

	luaL_pushresult(&b);
	return 1;
}

static const luaL_Reg l_base64[] = {
	{ "encode", base64_encode },
	{ "decode", base64_decode },
	{ NULL, NULL }
};

int luaopen_base64(lua_State * L)
{
	luaL_newlib (L, l_base64);
	return 1;
}
