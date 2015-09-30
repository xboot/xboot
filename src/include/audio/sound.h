#ifndef __SOUND_H__
#define __SOUND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <audio/audio.h>

struct sound_t
{
	/* Sound rate */
	enum audio_rate_t rate;

	/* Sound format */
	enum audio_format_t fmt;

	/* Sound channel */
	int channel;

	/* Sound position in samples */
	int position;

	/* Sound length in samples */
	int length;

	/* Sound seek in samples */
	int (*seek)(struct sound_t * sound, int offset);

	/* Sound read in samples */
	int (*read)(struct sound_t * sound, void * buf, int count);

	/* Sound close */
	void (*close)(struct sound_t * sound);

	/* Private data */
	void * priv;
};

struct sound_loader_t
{
	const char * ext;
	bool_t (*load)(struct sound_t * sound, const char * filename);
};

extern struct sound_loader_t __sound_loader_wav;

struct sound_t * sound_alloc(const char * filename);
int sound_seek(struct sound_t * sound, int offset);
int sound_read(struct sound_t * sound, void * buf, int count);
void sound_free(struct sound_t * sound);

#ifdef __cplusplus
}
#endif

#endif /* __SOUND_H__ */
