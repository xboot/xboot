#ifndef __UNALIGNED_H__
#define __UNALIGNED_H__

#include <stdint.h>

#define unaligned_get_type(type, ptr) ({				\
	const struct { type val; } __attribute__((packed))	\
	*_pptr = (typeof(_pptr))(ptr);						\
	_pptr->val;											\
})

#define unaligned_set_type(type, ptr, value) ({			\
	struct { type val; } __attribute__((packed))		\
	*_pptr = (typeof(_pptr))(ptr);						\
	_pptr->val = (value);								\
})

static inline uint16_t unaligned_get_u16(virtual_addr_t addr)
{
	return unaligned_get_type(uint16_t, addr);
}

static inline uint32_t unaligned_get_u32(virtual_addr_t addr)
{
	return unaligned_get_type(uint32_t, addr);
}

static inline uint64_t unaligned_get_u64(virtual_addr_t addr)
{
	return unaligned_get_type(uint64_t, addr);
}

static inline void unaligned_set_u16(virtual_addr_t addr, uint16_t val)
{
	unaligned_set_type(uint16_t, addr, val);
}

static inline void unaligned_set_u32(virtual_addr_t addr, uint32_t val)
{
	unaligned_set_type(uint32_t, addr, val);
}

static inline void unaligned_set_u64(virtual_addr_t addr, uint64_t val)
{
	unaligned_set_type(uint64_t, addr, val);
}

#endif /* __UNALIGNED_H__ */
