#ifndef __SDCARD_H__
#define __SDCARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct sdcard_t
{
	u32_t version;
	u32_t ocr;
	u32_t rca;
	u32_t cid[4];
	u32_t csd[4];
	u8_t extcsd[512];

	u32_t high_capacity;
	u32_t tran_speed;
	u32_t dsr_imp;
	u32_t read_bl_len;
	u32_t write_bl_len;
	u64_t capacity;
};

#ifdef __cplusplus
}
#endif

#endif /* __SDCARD_H__ */
