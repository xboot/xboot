#ifndef __IR_DECODER_NEC_H__
#define __IR_DECODER_NEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum ir_nec_state_t {
	IR_NEC_STATE_INACTIVE		= 0,
	IR_NEC_STATE_HEADER_SPACE	= 1,
	IR_NEC_STATE_BIT_PULSE		= 2,
	IR_NEC_STATE_BIT_SPACE		= 3,
	IR_NEC_STATE_TRAILER_PULSE	= 4,
	IR_NEC_STATE_TRAILER_SPACE	= 5,
};

struct ir_nec_decoder_t {
	enum ir_nec_state_t state;
	uint32_t bits;
	int count;
	int repeat;
};

uint32_t ir_nec_decoder_handle(struct ir_nec_decoder_t * decoder, int pulse, int duration);

#ifdef __cplusplus
}
#endif

#endif /* __IR_DECODER_NEC_H__ */
