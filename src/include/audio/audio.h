#ifndef __AUDIO_H__
#define __AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum pcm_format_t {
	PCM_FORMAT_BIT8		= 8,
	PCM_FORMAT_BIT16	= 16,
	PCM_FORMAT_BIT24	= 24,
	PCM_FORMAT_BIT32	= 32,
};

enum pcm_rate_t {
	PCM_RATE_8000		= 8000,
	PCM_RATE_11025		= 11025,
	PCM_RATE_16000		= 16000,
	PCM_RATE_22050		= 22050,
	PCM_RATE_32000		= 32000,
	PCM_RATE_44100		= 44100,
	PCM_RATE_48000		= 48000,
	PCM_RATE_64000		= 64000,
	PCM_RATE_88200		= 88200,
	PCM_RATE_96000		= 96000,
	PCM_RATE_176400		= 176400,
	PCM_RATE_192000		= 192000,
};

typedef int (*audio_callback_t)(void * data, void * buf, int count);

struct audio_t
{
	/* The audio name */
	char * name;

	/* Audio playback start */
	void (*playback_start)(struct audio_t * audio, enum pcm_rate_t rate, enum pcm_format_t fmt, int ch, audio_callback_t cb, void * data);

	/* Audio playback stop */
	void (*playback_stop)(struct audio_t * audio);

	/* Audio capture start */
	void (*capture_start)(struct audio_t * audio, enum pcm_rate_t rate, enum pcm_format_t fmt, int ch, audio_callback_t cb, void * data);

	/* Audio capture stop */
	void (*capture_stop)(struct audio_t * audio);

	/* Ioctl interface */
	int (*ioctl)(struct audio_t * audio, const char * cmd, void * arg);

	/* Private data */
	void * priv;
};

struct audio_t * search_audio(const char * name);
struct audio_t * search_first_audio(void);
struct device_t * register_audio(struct audio_t * audio, struct driver_t * drv);
void unregister_audio(struct audio_t * audio);

void audio_playback(struct audio_t * audio);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_H__ */
