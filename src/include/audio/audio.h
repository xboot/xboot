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

	/* Initialize the audio */
	void (*init)(struct audio_t * audio);

	/* Clean up the audio */
	void (*exit)(struct audio_t * audio);

	/* Audio playback start */
	void (*playback_start)(struct audio_t * audio, enum pcm_rate_t rate, enum pcm_format_t fmt, int ch, audio_callback_t cb, void * data);

	/* Audio playback stop */
	void (*playback_stop)(struct audio_t * audio);

	/* Audio capture start */
	void (*capture_start)(struct audio_t * audio, enum pcm_rate_t rate, enum pcm_format_t fmt, int ch, audio_callback_t cb, void * data);

	/* Audio capture stop */
	void (*capture_stop)(struct audio_t * audio);

	/* Suspend audio */
	void (*suspend)(struct audio_t * audio);

	/* Resume audio */
	void (*resume)(struct audio_t * audio);

	/* Private data */
	void * priv;
};

struct audio_t * search_audio(const char * name);
struct audio_t * search_first_audio(void);
bool_t register_audio(struct audio_t * audio);
bool_t unregister_audio(struct audio_t * audio);
void audio_playback_start(struct audio_t * audio);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_H__ */
