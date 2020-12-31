#include <x.h>
#include <sandbox.h>

struct sandbox_audio_context_t {
	pthread_t thread;
	snd_pcm_t * handle;
	snd_pcm_hw_params_t * params;
	snd_pcm_format_t format;
	snd_pcm_uframes_t frames;
	unsigned int bytes_per_frame;
	void * buffer;
	int buflen;
	int(*cb)(void *, void *, int);
	void * data;
	int running;
};

static void * sandbox_audio_playback_thread(void * arg)
{
	struct sandbox_audio_context_t * ctx = (struct sandbox_audio_context_t *)arg;
	int len, ret;

	while(ctx->running)
	{
		len = ctx->cb(ctx->data, ctx->buffer, ctx->buflen) / ctx->bytes_per_frame;
		if(len > 0)
		{
			while((ret = snd_pcm_writei(ctx->handle, ctx->buffer, len)) < 0)
			{
				if(ret == -EPIPE)
				{
					snd_pcm_prepare(ctx->handle);
				}
				else
				{
					ctx->running = 0;
					break;
				}
			}
		}
		else
		{
			ctx->running = 0;
			break;
		}
	}
	pthread_exit(NULL);
	return NULL;
}

void * sandbox_audio_playback_start(int rate, int fmt, int ch, int(*cb)(void *, void *, int), void * data)
{
	struct sandbox_audio_context_t * ctx;
	unsigned int val = rate;
	unsigned int buffer_time;
	unsigned int period_time;
	int dir = 0;

	ctx = malloc(sizeof(struct sandbox_audio_context_t));
	if(!ctx)
		return NULL;
	memset(ctx, 0, sizeof(struct sandbox_audio_context_t));

	if(snd_pcm_open(&ctx->handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0)
	{
		free(ctx);
		return NULL;
	}
	snd_pcm_hw_params_alloca(&ctx->params);
	if(snd_pcm_hw_params_any(ctx->handle, ctx->params) < 0)
	{
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params_set_access(ctx->handle, ctx->params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
	{
		free(ctx);
		return NULL;
	}
	switch(fmt)
	{
	case 8:
		ctx->format = SND_PCM_FORMAT_S8;
		break;
	case 16:
		ctx->format = SND_PCM_FORMAT_S16_LE;
		break;
	case 24:
		ctx->format = SND_PCM_FORMAT_S24_LE;
		break;
	case 32:
		ctx->format = SND_PCM_FORMAT_S32_LE;
		break;
	default:
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params_set_format(ctx->handle, ctx->params, ctx->format) < 0)
	{
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params_set_channels(ctx->handle, ctx->params, ch) < 0)
	{
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params_set_rate_near(ctx->handle, ctx->params, &val, &dir) < 0)
	{
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params_get_buffer_time_max(ctx->params, &buffer_time, 0) < 0)
	{
		free(ctx);
		return NULL;
	}
	if(buffer_time > 100000)
		buffer_time = 100000;
	period_time = buffer_time / 4;
	if(snd_pcm_hw_params_set_period_time_near(ctx->handle, ctx->params, &period_time, 0) < 0)
	{
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params_set_buffer_time_near(ctx->handle, ctx->params, &buffer_time, 0) < 0)
	{
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params(ctx->handle, ctx->params) < 0)
	{
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params_get_period_size(ctx->params, &ctx->frames, &dir) < 0)
	{
		free(ctx);
		return NULL;
	}
	ctx->bytes_per_frame = ch * snd_pcm_format_width(ctx->format) / 8;
	ctx->buflen = ctx->frames * ctx->bytes_per_frame;
	ctx->buffer = malloc(ctx->buflen);
	if(!ctx->buffer)
	{
		free(ctx);
		return NULL;
	}
	ctx->data = data;
	ctx->cb = cb;
	if(!ctx->cb)
	{
		free(ctx->buffer);
		free(ctx);
		return NULL;
	}
	ctx->running = 1;
	if(pthread_create(&ctx->thread, NULL, sandbox_audio_playback_thread, ctx) < 0)
	{
		free(ctx->buffer);
		free(ctx);
		return NULL;
	}
	return ctx;
}

void sandbox_audio_playback_stop(void * context)
{
	struct sandbox_audio_context_t * ctx = (struct sandbox_audio_context_t *)context;
	void * ret;

	if(ctx)
	{
		ctx->running = 0;
		pthread_join(ctx->thread, &ret);
		snd_pcm_drop(ctx->handle);
		snd_pcm_close(ctx->handle);
		free(ctx->buffer);
		free(ctx);
	}
}

int sandbox_audio_playback_status(void * context)
{
	struct sandbox_audio_context_t * ctx = (struct sandbox_audio_context_t *)context;
	if(ctx)
		return ctx->running;
	return 0;
}

static void * sandbox_audio_capture_thread(void * arg)
{
	struct sandbox_audio_context_t * ctx = (struct sandbox_audio_context_t *)arg;
	int len, l, ret;

	while(ctx->running)
	{
		ret = snd_pcm_readi(ctx->handle, ctx->buffer, ctx->frames);
		if(ret > 0)
		{
			len = ret * ctx->bytes_per_frame;
			l = 0;
			do {
				l += ctx->cb(ctx->data, ctx->buffer + l, len - l);
			} while(l < len);
		}
		else if(ret == -EPIPE)
		{
			snd_pcm_prepare(ctx->handle);
		}
		else
		{
			ctx->running = 0;
			break;
		}
	}
	pthread_exit(NULL);
	return NULL;
}

void * sandbox_audio_capture_start(int rate, int fmt, int ch, int(*cb)(void *, void *, int), void * data)
{
	struct sandbox_audio_context_t * ctx;
	unsigned int val = rate;
	int dir = 0;

	ctx = malloc(sizeof(struct sandbox_audio_context_t));
	if(!ctx)
		return NULL;
	memset(ctx, 0, sizeof(struct sandbox_audio_context_t));

	if(snd_pcm_open(&ctx->handle, "default", SND_PCM_STREAM_CAPTURE, 0) < 0)
	{
		free(ctx);
		return NULL;
	}
	snd_pcm_hw_params_alloca(&ctx->params);
	if(snd_pcm_hw_params_any(ctx->handle, ctx->params) < 0)
	{
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params_set_access(ctx->handle, ctx->params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
	{
		free(ctx);
		return NULL;
	}
	switch(fmt)
	{
	case 8:
		ctx->format = SND_PCM_FORMAT_S8;
		break;
	case 16:
		ctx->format = SND_PCM_FORMAT_S16_LE;
		break;
	case 24:
		ctx->format = SND_PCM_FORMAT_S24_LE;
		break;
	case 32:
		ctx->format = SND_PCM_FORMAT_S32_LE;
		break;
	default:
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params_set_format(ctx->handle, ctx->params, ctx->format) < 0)
	{
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params_set_channels(ctx->handle, ctx->params, ch) < 0)
	{
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params_set_rate_near(ctx->handle, ctx->params, &val, &dir) < 0)
	{
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params(ctx->handle, ctx->params) < 0)
	{
		free(ctx);
		return NULL;
	}
	if(snd_pcm_hw_params_get_period_size(ctx->params, &ctx->frames, &dir) < 0)
	{
		free(ctx);
		return NULL;
	}
	ctx->bytes_per_frame = ch * snd_pcm_format_width(ctx->format) / 8;
	ctx->buflen = ctx->frames * ctx->bytes_per_frame;
	ctx->buffer = malloc(ctx->buflen);
	if(!ctx->buffer)
	{
		free(ctx);
		return NULL;
	}
	ctx->data = data;
	ctx->cb = cb;
	if(!ctx->cb)
	{
		free(ctx->buffer);
		free(ctx);
		return NULL;
	}
	ctx->running = 1;
	if(pthread_create(&ctx->thread, NULL, sandbox_audio_capture_thread, ctx) < 0)
	{
		free(ctx->buffer);
		free(ctx);
		return NULL;
	}
	return ctx;
}

void sandbox_audio_capture_stop(void * context)
{
	struct sandbox_audio_context_t * ctx = (struct sandbox_audio_context_t *)context;
	void * ret;

	if(ctx)
	{
		ctx->running = 0;
		pthread_join(ctx->thread, &ret);
		snd_pcm_drop(ctx->handle);
		snd_pcm_close(ctx->handle);
		free(ctx->buffer);
		free(ctx);
	}
}

int sandbox_audio_capture_status(void * context)
{
	struct sandbox_audio_context_t * ctx = (struct sandbox_audio_context_t *)context;
	if(ctx)
		return ctx->running;
	return 0;
}

void sandbox_audio_set_playback_volume(int vol)
{
	snd_mixer_t * handle;
	snd_mixer_selem_id_t * sid;
	snd_mixer_elem_t * elem;
	const char * card = "default";
	const char * name = "Master";
	long minvol, maxvol;

	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, card);
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, name);
	elem = snd_mixer_find_selem(handle, sid);

	snd_mixer_selem_get_playback_volume_range(elem, &minvol, &maxvol);
	snd_mixer_selem_set_playback_volume_all(elem, vol * (maxvol - minvol) / 1000);
	snd_mixer_close(handle);
}

int sandbox_audio_get_playback_volume(void)
{
	snd_mixer_t * handle;
	snd_mixer_selem_id_t * sid;
	snd_mixer_elem_t * elem;
	const char * card = "default";
	const char * name = "Master";
	long minvol, maxvol;
	long vol;

	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, card);
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, name);
	elem = snd_mixer_find_selem(handle, sid);

	snd_mixer_selem_get_playback_volume_range(elem, &minvol, &maxvol);
	snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &vol);
	snd_mixer_close(handle);

	vol = vol * 1000 / (maxvol - minvol);
	if(vol < 0)
		vol = 0;
	else if(vol > 1000)
		vol = 1000;
	return (int)vol;
}

void sandbox_audio_set_capture_volume(int vol)
{
	snd_mixer_t * handle;
	snd_mixer_selem_id_t * sid;
	snd_mixer_elem_t * elem;
	const char * card = "default";
	const char * name = "Capture";
	long minvol, maxvol;

	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, card);
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, name);
	elem = snd_mixer_find_selem(handle, sid);

	snd_mixer_selem_get_capture_volume_range(elem, &minvol, &maxvol);
	snd_mixer_selem_set_capture_volume_all(elem, vol * (maxvol - minvol) / 1000);
	snd_mixer_close(handle);
}

int sandbox_audio_get_capture_volume(void)
{
	snd_mixer_t * handle;
	snd_mixer_selem_id_t * sid;
	snd_mixer_elem_t * elem;
	const char * card = "default";
	const char * name = "Capture";
	long minvol, maxvol;
	long vol;

	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, card);
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, name);
	elem = snd_mixer_find_selem(handle, sid);

	snd_mixer_selem_get_capture_volume_range(elem, &minvol, &maxvol);
	snd_mixer_selem_get_capture_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &vol);
	snd_mixer_close(handle);

	vol = vol * 1000 / (maxvol - minvol);
	if(vol < 0)
		vol = 0;
	else if(vol > 1000)
		vol = 1000;
	return (int)vol;
}
