#ifndef __AUDIO_H__
#define __AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <audio/sound.h>

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
