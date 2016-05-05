#ifndef __ROTARY_ENCODER_H__
#define __ROTARY_ENCODER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <input/input.h>
#include <input/keyboard.h>

struct rotary_encoder_data_t
{
	int gpio_a;
	int gpio_b;
	int gpio_c;
	int inverted_a;
	int inverted_b;
	int inverted_c;
	int step;
};

#ifdef __cplusplus
}
#endif

#endif /* __ROTARY_ENCODER_H__ */
