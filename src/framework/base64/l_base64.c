/*
 * framework/base64/l_base64.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <framework/base64/l_base64.h>

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
