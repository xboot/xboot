/*
 * framework/core/l-xfs.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <framework/core/l-xfs.h>

static int l_xfs_open(lua_State * L)
{
	struct xfs_context_t * ctx = ((struct vmctx_t *)luahelper_vmctx(L))->xfs;
	const char * name = luaL_checkstring(L, 1);
	const char * type = luaL_optstring(L, 2, "r");
	struct xfs_file_t * file = NULL;
	if(strcmp(type, "r") == 0)
		file = xfs_open_read(ctx, name);
	else if(strcmp(type, "w") == 0)
		file = xfs_open_write(ctx, name);
	else if(strcmp(type, "a") == 0)
		file = xfs_open_append(ctx, name);
	if(!file)
		return 0;
	struct lxfsfile_t * f = lua_newuserdata(L, sizeof(struct lxfsfile_t));
	f->file = file;
	luaL_setmetatable(L, MT_XFS_FILE);
	return 1;
}

static int l_xfs_isdir(lua_State * L)
{
	struct xfs_context_t * ctx = ((struct vmctx_t *)luahelper_vmctx(L))->xfs;
	const char * name = luaL_checkstring(L, 1);
	lua_pushboolean(L, xfs_isdir(ctx, name));
	return 1;
}

static int l_xfs_isfile(lua_State * L)
{
	struct xfs_context_t * ctx = ((struct vmctx_t *)luahelper_vmctx(L))->xfs;
	const char * name = luaL_checkstring(L, 1);
	lua_pushboolean(L, xfs_isfile(ctx, name));
	return 1;
}

static int l_xfs_mkdir(lua_State * L)
{
	struct xfs_context_t * ctx = ((struct vmctx_t *)luahelper_vmctx(L))->xfs;
	const char * name = luaL_checkstring(L, 1);
	lua_pushboolean(L, xfs_mkdir(ctx, name));
	return 1;
}

static int l_xfs_remove(lua_State * L)
{
	struct xfs_context_t * ctx = ((struct vmctx_t *)luahelper_vmctx(L))->xfs;
	const char * name = luaL_checkstring(L, 1);
	lua_pushboolean(L, xfs_remove(ctx, name));
	return 1;
}

static const luaL_Reg l_xfs[] = {
	{"open",	l_xfs_open},
	{"isdir",	l_xfs_isdir},
	{"isfile",	l_xfs_isfile},
	{"mkdir",	l_xfs_mkdir},
	{"remove",	l_xfs_remove},
	{NULL,		NULL}
};

static int m_xfs_file_gc(lua_State * L)
{
	struct lxfsfile_t * f = luaL_checkudata(L, 1, MT_XFS_FILE);
	if(f->file)
	{
		xfs_close(f->file);
		f->file = NULL;
	}
	return 0;
}

static int xfs_file_test_eof(lua_State * L, struct lxfsfile_t * f)
{
	lua_pushliteral(L, "");
	return (xfs_tell(f->file) >= xfs_length(f->file)) ? 1 : 0;
}

static int xfs_file_read_chars(lua_State * L, struct lxfsfile_t * f, s64_t l)
{
	luaL_Buffer b;
	char * p;
	s64_t n;

	luaL_buffinit(L, &b);
	p = luaL_prepbuffsize(&b, l);
	n = xfs_read(f->file, p, l);
	luaL_addsize(&b, n);
	luaL_pushresult(&b);
	return (n > 0);
}

static int xfs_file_read_line(lua_State * L, struct lxfsfile_t * f, int chop)
{
	luaL_Buffer b;
	char * buf, c = '\0';
	int i;

	luaL_buffinit(L, &b);
	while(c != '\n')
	{
		buf = luaL_prepbuffer(&b);
		i = 0;
		while((i < LUAL_BUFFERSIZE) && (xfs_read(f->file, &c, 1) == 1) && (c != '\n'))
			buf[i++] = c;
		luaL_addsize(&b, i);
	}
	if(!chop && (c == '\n'))
		luaL_addchar(&b, c);
	luaL_pushresult(&b);
	return ((c == '\n') || (lua_rawlen(L, -1) > 0));
}

static int xfs_file_read_all(lua_State * L, struct lxfsfile_t * f)
{
	luaL_Buffer b;
	char * p;
	s64_t n;

	luaL_buffinit(L, &b);
	do
	{
		p = luaL_prepbuffer(&b);
		n = xfs_read(f->file, p, LUAL_BUFFERSIZE);
		luaL_addsize(&b, n);
	} while(n == LUAL_BUFFERSIZE);
	luaL_pushresult(&b);
	return 1;
}

static int m_xfs_file_read(lua_State * L)
{
	struct lxfsfile_t * f = luaL_checkudata(L, 1, MT_XFS_FILE);
	int nargs = lua_gettop(L) - 1;
	int first = 2;
	int success;
	int n;

	if(nargs == 0)
	{
		success = xfs_file_read_line(L, f, 1);
		n = first + 1;
	}
	else
	{
		luaL_checkstack(L, nargs + LUA_MINSTACK, "too many arguments");
		success = 1;
		for(n = first; nargs-- && success; n++)
		{
			if(lua_type(L, n) == LUA_TNUMBER)
			{
				s64_t l = (s64_t)luaL_checkinteger(L, n);
				success = (l == 0) ? xfs_file_test_eof(L, f) : xfs_file_read_chars(L, f, l);
			}
			else
			{
				const char * q = luaL_checkstring(L, n);
				if(*q == '*')
					q++;
				switch(*q)
				{
				case 'n':
					success = 0;
					break;
				case 'l':
					success = xfs_file_read_line(L, f, 1);
					break;
				case 'L':
					success = xfs_file_read_line(L, f, 0);
					break;
				case 'a':
					success = xfs_file_read_all(L, f);
					break;
				default:
					return luaL_argerror(L, n, "invalid format");
				}
			}
		}
	}
	if(!success)
	{
		lua_pop(L, 1);
		lua_pushnil(L);
	}
	return n - first;
}

static int m_xfs_file_write(lua_State * L)
{
	return 0;
}

static int m_xfs_file_seek(lua_State * L)
{
	struct lxfsfile_t * f = luaL_checkudata(L, 1, MT_XFS_FILE);
	s64_t offset = luaL_optinteger(L, 2, 0);
	lua_pushinteger(L, xfs_seek(f->file, offset));
	return 1;
}

static int m_xfs_file_tell(lua_State * L)
{
	struct lxfsfile_t * f = luaL_checkudata(L, 1, MT_XFS_FILE);
	lua_pushinteger(L, xfs_tell(f->file));
	return 1;
}

static int m_xfs_file_length(lua_State * L)
{
	struct lxfsfile_t * f = luaL_checkudata(L, 1, MT_XFS_FILE);
	lua_pushinteger(L, xfs_length(f->file));
	return 1;
}

static int m_xfs_file_close(lua_State * L)
{
	struct lxfsfile_t * f = luaL_checkudata(L, 1, MT_XFS_FILE);
	if(f->file)
	{
		xfs_close(f->file);
		f->file = NULL;
	}
	return 0;
}

static const luaL_Reg m_xfs_file[] = {
	{"__gc",	m_xfs_file_gc},
	{"read",	m_xfs_file_read},
	{"write",	m_xfs_file_write},
	{"seek",	m_xfs_file_seek},
	{"tell",	m_xfs_file_tell},
	{"length",	m_xfs_file_length},
	{"close",	m_xfs_file_close},
	{NULL,		NULL}
};

int luaopen_xfs(lua_State * L)
{
	luaL_newlib(L, l_xfs);
	luahelper_create_metatable(L, MT_XFS_FILE, m_xfs_file);
	return 1;
}
