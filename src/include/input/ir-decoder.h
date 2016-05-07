#ifndef __IR_DECODER_H__
#define __IR_DECODER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <input/input.h>
#include <input/keyboard.h>

enum decoder_state_t {
	STATE_INACTIVE		= 0,
	STATE_HEADER_SPACE	= 1,
	STATE_BIT_PULSE		= 2,
	STATE_BIT_SPACE		= 3,
	STATE_TRAILER_PULSE	= 4,
	STATE_TRAILER_SPACE	= 5,
};

struct ir_decoder_t {
	enum decoder_state_t state;
	int count;
	int repeat;
	uint32_t bits;
};

static inline int geq_margin(int d1, int d2, int margin)
{
	return (d1 > (d2 - margin)) ? 1 : 0;
}

static inline int eq_margin(int d1, int d2, int margin)
{
	return ((d1 > (d2 - margin)) && (d1 < (d2 + margin))) ? 1 : 0;
}

#ifdef __cplusplus
}
#endif

#endif /* __IR_DECODER_H__ */
