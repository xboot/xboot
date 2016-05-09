#ifndef __RC_DECODER_NEC_H__
#define __RC_DECODER_NEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum rc_state_nec_t {
	RC_STATE_NEC_INACTIVE		= 0,
	RC_STATE_NEC_HEADER_SPACE	= 1,
	RC_STATE_NEC_BIT_PULSE		= 2,
	RC_STATE_NEC_BIT_SPACE		= 3,
	RC_STATE_NEC_TRAILER_PULSE	= 4,
	RC_STATE_NEC_TRAILER_SPACE	= 5,
};

struct rc_decoder_nec_t {
	enum rc_state_nec_t state;
	uint32_t bits;
	int count;
	int repeat;
};

uint32_t rc_nec_decoder_handle(struct rc_decoder_nec_t * decoder, int pulse, int duration);

#ifdef __cplusplus
}
#endif

#endif /* __RC_DECODER_NEC_H__ */
