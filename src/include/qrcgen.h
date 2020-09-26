#ifndef __QRCGEN_H__
#define __QRCGEN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define QRCGEN_VERSION_MIN		(1)
#define QRCGEN_VERSION_MAX		(40)
#define QRCGEN_BUFFER_LEN(v)	((((v) * 4 + 17) * ((v) * 4 + 17) + 7) / 8 + 1)
#define QRCGEN_BUFFER_LEN_MAX	QRCGEN_BUFFER_LEN(QRCGEN_VERSION_MAX)

enum qrcgen_ecc_t {
	QRCGEN_ECC_LOW				= 0,
	QRCGEN_ECC_MEDIUM			= 1,
	QRCGEN_ECC_QUARTILE			= 2,
	QRCGEN_ECC_HIGH				= 3,
};

enum qrcgen_mask_t {
	QRCGEN_MASK_AUTO			= -1,
	QRCGEN_MASK_0				= 0,
	QRCGEN_MASK_1				= 1,
	QRCGEN_MASK_2				= 2,
	QRCGEN_MASK_3				= 3,
	QRCGEN_MASK_4				= 4,
	QRCGEN_MASK_5				= 5,
	QRCGEN_MASK_6				= 6,
	QRCGEN_MASK_7				= 7,
};

enum qrcgen_mode_t {
	QRCGEN_MODE_NUMERIC			= 0x1,
	QRCGEN_MODE_ALPHANUMERIC	= 0x2,
	QRCGEN_MODE_BYTE			= 0x4,
	QRCGEN_MODE_KANJI			= 0x8,
	QRCGEN_MODE_ECI				= 0x7,
};

struct qrcgen_segment_t {
	enum qrcgen_mode_t mode;
	int nchar;
	uint8_t * data;
	int blen;
};

int qrcgen_encode_text(const char * txt, uint8_t * tmp, uint8_t * qrc, enum qrcgen_ecc_t ecc, int minv, int maxv, enum qrcgen_mask_t mask, int boost);
int qrcgen_encode_binary(uint8_t * buf, int len, uint8_t * qrc, enum qrcgen_ecc_t ecc, int minv, int maxv, enum qrcgen_mask_t mask, int boost);
int qrcgen_get_size(uint8_t * qrc);
int qrcgen_get_pixel(uint8_t * qrc, int x, int y);

#ifdef __cplusplus
}
#endif

#endif /* __QRCGEN_H__ */
