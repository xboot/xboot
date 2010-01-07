#ifndef __MACROS_H__
#define __MACROS_H__

#include <configs.h>
#include <default.h>

/**
 * define the macro of size.
 */
#define SZ_16						(0x00000010)
#define SZ_256						(0x00000100)
#define SZ_512						(0x00000200)

#define SZ_1K						(0x00000400)
#define SZ_4K						(0x00001000)
#define SZ_8K						(0x00002000)
#define SZ_16K						(0x00004000)
#define SZ_32K						(0x00008000)
#define SZ_64K						(0x00010000)
#define SZ_128K						(0x00020000)
#define SZ_256K						(0x00040000)
#define SZ_512K						(0x00080000)

#define SZ_1M						(0x00100000)
#define SZ_2M						(0x00200000)
#define SZ_4M						(0x00400000)
#define SZ_8M						(0x00800000)
#define SZ_16M						(0x01000000)
#define SZ_32M						(0x02000000)
#define SZ_64M						(0x04000000)
#define SZ_128M						(0x08000000)
#define SZ_256M						(0x10000000)
#define SZ_512M						(0x20000000)

#define SZ_1G						(0x40000000)
#define SZ_2G						(0x80000000)

#define ARRAY_SIZE(array)			( sizeof(array) / sizeof((array)[0]) )

/**
 * convenience functions for memory sizes.
 */
#define KB(x)						((typeof (x)) ((x_u64) x * 1024))
#define MB(x)						((typeof (x)) ((x_u64) x * 1024*1024))
#define GB(x)						((typeof (x)) ((x_u64) x * 1024*1024*1024))

/**
 * define system width, depend the type of x_sys.
 */
#define BITS_OF_SYS					(sizeof(x_sys)*8)

/**
 * offset for struct fields.
 */
#if (__GNUC__ >= 4)
#define offsetof(type, member)		__builtin_offsetof(type, member)
#else
#define offsetof(type, field)		((x_sys)(&((type *) 0)->field))
#endif

/*
 * memory align
 */
#define MEM_ALIGNMENT				(sizeof(x_sys))
#define MEM_ALIGN_SIZE(size)		(((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))
#define MEM_ALIGN(addr)				((void *)(((x_sys)(addr) + MEM_ALIGNMENT - 1) & ~(x_sys)(MEM_ALIGNMENT-1)))

/**
 * define the macro of bit field.
 */
#if	( defined(__LITTLE_ENDIAN) && defined(__BIG_ENDIAN) )
#error "please don't define __LITTLE_ENDIAN and __BIG_ENDIAN at the same time";
#elif defined(__BIG_ENDIAN)
#define BITFIELD2(t,a,b)								t b; t a;
#define BITFIELD3(t,a,b,c)								t c; t b; t a;
#define BITFIELD4(t,a,b,c,d)							t d; t c; t b; t a;
#define BITFIELD5(t,a,b,c,d,e)							t e; t d; t c; t b; t a;
#define BITFIELD6(t,a,b,c,d,e,f)						t f; t e; t d; t c; t b; t a;
#define BITFIELD7(t,a,b,c,d,e,f,g)						t g; t f; t e; t d; t c; t b; t a;
#define BITFIELD8(t,a,b,c,d,e,f,g,h)					t h; t g; t f; t e; t d; t c; t b; t a;
#define BITFIELD9(t,a,b,c,d,e,f,g,h,i)					t i; t h; t g; t f; t e; t d; t c; t b; t a;
#define BITFIELD10(t,a,b,c,d,e,f,g,h,i,j) 				t j; t i; t h; t g; t f; t e; t d; t c; t b; t a;
#define BITFIELD11(t,a,b,c,d,e,f,g,h,i,j,k) 			t k; t j; t i; t h; t g; t f; t e; t d; t c; t b; t a;
#define BITFIELD12(t,a,b,c,d,e,f,g,h,i,j,k,l)			t l; t k; t j; t i; t h; t g; t f; t e; t d; t c; t b; t a;
#define BITFIELD13(t,a,b,c,d,e,f,g,h,i,j,k,l,m)			t m; t l; t k; t j; t i; t h; t g; t f; t e; t d; t c; t b; t a;
#define BITFIELD17(t,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q)	t q; t p; t o; t n; t m; t l; t k; t j;t i; t h; t g; t f; t e; t d; t c; t b; t a;
#define SHUFFLE2(a,b)									b,a
#define SHUFFLE3(a,b,c)									c,b,a
#define SHUFFLE4(a,b,c,d)								d,c,b,a
#define SHUFFLE5(a,b,c,d,e)								e,d,c,b,a
#define SHUFFLE6(a,b,c,d,e,f)							f,e,d,c,b,a
#define SHUFFLE7(a,b,c,d,e,f,g)							g,f,e,d,c,b,a
#elif defined(__LITTLE_ENDIAN)
#define BITFIELD2(t,a,b)								t a; t b;
#define BITFIELD3(t,a,b,c)								t a; t b; t c;
#define BITFIELD4(t,a,b,c,d)							t a; t b; t c; t d;
#define BITFIELD5(t,a,b,c,d,e)							t a; t b; t c; t d; t e;
#define BITFIELD6(t,a,b,c,d,e,f)						t a; t b; t c; t d; t e; t f;
#define BITFIELD7(t,a,b,c,d,e,f,g)						t a; t b; t c; t d; t e; t f; t g;
#define BITFIELD8(t,a,b,c,d,e,f,g,h)					t a; t b; t c; t d; t e; t f; t g; t h;
#define BITFIELD9(t,a,b,c,d,e,f,g,h,i)					t a; t b; t c; t d; t e; t f; t g; t h; t i;
#define BITFIELD10(t,a,b,c,d,e,f,g,h,i,j) 				t a; t b; t c; t d; t e; t f; t g; t h; t i; t j;
#define BITFIELD11(t,a,b,c,d,e,f,g,h,i,j,k) 			t a; t b; t c; t d; t e; t f; t g; t h; t i; t j; t k;
#define BITFIELD12(t,a,b,c,d,e,f,g,h,i,j,k,l) 			t a; t b; t c; t d; t e; t f; t g; t h; t i; t j; t k; t l;
#define BITFIELD13(t,a,b,c,d,e,f,g,h,i,j,k,l,m) 		t a; t b; t c; t d; t e; t f; t g; t h; t i; t j; t k; t l; t m;
#define BITFIELD17(t,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) t a; t b; t c; t d; t e; t f; t g; t h; t i; t j; t k; t l; t m; t n; t o; t p; t q;
#define SHUFFLE2(a,b)									a,b
#define SHUFFLE3(a,b,c)									a,b,c
#define SHUFFLE4(a,b,c,d)								a,b,c,d
#define SHUFFLE5(a,b,c,d,e)								a,b,c,d,e
#define SHUFFLE6(a,b,c,d,e,f)							a,b,c,d,e,f
#define SHUFFLE7(a,b,c,d,e,f,g)							a,b,c,d,e,f,g
#else
#error "you must define __LITTLE_ENDIAN or __BIG_ENDIAN before continue";
#endif

#endif /* __MACROS_H__ */
