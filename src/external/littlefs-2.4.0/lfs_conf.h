#ifndef __LFS_CONF_H__
#define __LFS_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

#define LFS_TRACE(...)
#define LFS_DEBUG(...)
#define LFS_WARN(...)
#define LFS_ERROR(...)

#define LFS_NO_ASSERT
#define LFS_ASSERT(x)

#define LFS_THREADSAFE

static inline uint32_t lfs_max(uint32_t a, uint32_t b)
{
	return (a > b) ? a : b;
}

static inline uint32_t lfs_min(uint32_t a, uint32_t b)
{
	return (a < b) ? a : b;
}

static inline uint32_t lfs_aligndown(uint32_t a, uint32_t alignment)
{
	return a - (a % alignment);
}

static inline uint32_t lfs_alignup(uint32_t a, uint32_t alignment)
{
	return lfs_aligndown(a + alignment - 1, alignment);
}

static inline uint32_t lfs_npw2(uint32_t a)
{
	uint32_t r = 0;
	uint32_t s;
	a -= 1;
	s = (a > 0xffff) << 4; a >>= s; r |= s;
	s = (a > 0xff  ) << 3; a >>= s; r |= s;
	s = (a > 0xf   ) << 2; a >>= s; r |= s;
	s = (a > 0x3   ) << 1; a >>= s; r |= s;
	return (r | (a >> 1)) + 1;
}

static inline uint32_t lfs_ctz(uint32_t a)
{
	return lfs_npw2((a & -a) + 1) - 1;
}

static inline uint32_t lfs_popc(uint32_t a)
{
	a = a - ((a >> 1) & 0x55555555);
	a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
	return (((a + (a >> 4)) & 0xf0f0f0f) * 0x1010101) >> 24;
}

static inline int lfs_scmp(uint32_t a, uint32_t b)
{
	return (int)(unsigned)(a - b);
}

static inline uint32_t lfs_fromle32(uint32_t a) {
	return	(((uint8_t*)&a)[0] <<  0) |
			(((uint8_t*)&a)[1] <<  8) |
			(((uint8_t*)&a)[2] << 16) |
			(((uint8_t*)&a)[3] << 24);
}

static inline uint32_t lfs_tole32(uint32_t a)
{
	return lfs_fromle32(a);
}

static inline uint32_t lfs_frombe32(uint32_t a)
{
	return	(((uint8_t*)&a)[0] << 24) |
			(((uint8_t*)&a)[1] << 16) |
			(((uint8_t*)&a)[2] <<  8) |
			(((uint8_t*)&a)[3] <<  0);
}

static inline uint32_t lfs_tobe32(uint32_t a)
{
	return lfs_frombe32(a);
}

static inline void * lfs_malloc(size_t size)
{
	return malloc(size);
}

static inline void lfs_free(void * p)
{
	free(p);
}

uint32_t lfs_crc(uint32_t crc, const void * buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* __LFS_CONF_H__ */
