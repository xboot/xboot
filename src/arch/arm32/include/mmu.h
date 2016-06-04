#ifndef __ARM32_MMU_H__
#define __ARM32_MMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum {
	MAP_TYPE_NCNB	= 0x0,
	MAP_TYPE_NCB	= 0x1,
	MAP_TYPE_CNB	= 0x2,
	MAP_TYPE_CB		= 0x3,
};

void mmu_setup(const struct mmap_t * map);

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_MMU_H__ */
