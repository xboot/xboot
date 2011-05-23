#ifndef __LZIO_H__
#define __LZIO_H__

#include <lua/luacfg.h>
#include <lua/llimits.h>
#include <lua/lua.h>
#include <lua/lmem.h>

#define char2int(c)					cast(int, cast(unsigned char, (c)))
#define zgetc(z)					(((z)->n--)>0 ?  char2int(*(z)->p++) : luaZ_fill(z))

#define luaZ_initbuffer(L, buff)	((buff)->buffer = NULL, (buff)->buffsize = 0)
#define luaZ_buffer(buff)			((buff)->buffer)
#define luaZ_sizebuffer(buff)		((buff)->buffsize)
#define luaZ_bufflen(buff)			((buff)->n)
#define luaZ_resetbuffer(buff)		((buff)->n = 0)

#define luaZ_resizebuffer(L, buff, size)	\
	(luaM_reallocvector(L, (buff)->buffer, (buff)->buffsize, size, char), (buff)->buffsize = size)

#define luaZ_freebuffer(L, buff)	luaZ_resizebuffer(L, buff, 0)

#define EOZ							(-1)


typedef struct Mbuffer {
	char * buffer;
	size_t n;
	size_t buffsize;
} Mbuffer;

typedef struct Zio {
	size_t n;				/* bytes still unread */
	const char * p;			/* current position in buffer */
	lua_Reader reader;
	void * data;			/* additional data */
	lua_State * L;			/* Lua state (for reader) */
} ZIO;

int luaZ_fill(ZIO * z);
int luaZ_lookahead(ZIO * z);
void luaZ_init(lua_State * L, ZIO * z, lua_Reader reader, void * data);
size_t luaZ_read(ZIO * z, void * b, size_t n);
char * luaZ_openspace(lua_State * L, Mbuffer * buff, size_t n);

#endif /* __LZIO_H__ */
