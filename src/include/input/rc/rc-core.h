#ifndef __RC_CORE_H__
#define __RC_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <input/input.h>
#include <input/keyboard.h>
#include <input/rc/rc-decoder-nec.h>

struct rc_map_t {
	uint32_t scancode;
	uint32_t keycode;
};

struct rc_decoder_t {
	struct rc_decoder_nec_t nec;
	struct rc_map_t * map;
	int size;
};

uint32_t rc_decoder_handle(struct rc_decoder_t * decoder, int pulse, int duration);

#ifdef __cplusplus
}
#endif

#endif /* __RC_CORE_H__ */
