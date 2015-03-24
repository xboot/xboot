#ifndef __SANDBOX_LED_H__
#define __SANDBOX_LED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <led/led.h>
#include <sandbox.h>

struct sandbox_led_data_t
{
	const char * path;
};

#ifdef __cplusplus
}
#endif

#endif /* __SANDBOX_LED_H__ */
