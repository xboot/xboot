#ifndef __SDCARD_H__
#define __SDCARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct sdcard_t
{
	uint32_t version;
	uint32_t ocr;
	uint32_t rca;
	uint32_t cid[4];
	uint32_t csd[4];
};

#ifdef __cplusplus
}
#endif

#endif /* __SDCARD_H__ */
