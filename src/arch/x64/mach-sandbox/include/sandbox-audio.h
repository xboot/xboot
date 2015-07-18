#ifndef __SANDBOX_AUDIO_H__
#define __SANDBOX_AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <audio/audio.h>
#include <sandbox.h>

struct sandbox_audio_data_t
{
	int maxidle;
};

#ifdef __cplusplus
}
#endif

#endif /* __SANDBOX_AUDIO_H__ */
