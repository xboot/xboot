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
};

#ifdef __cplusplus
}
#endif

#endif /* __SDCARD_H__ */
