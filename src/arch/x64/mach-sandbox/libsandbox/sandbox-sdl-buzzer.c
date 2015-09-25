#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL.h>
#include <sandbox.h>

struct buzzer_callback_data_t
{
	int frequency;
	int amplitude;
	int sample;
	int channel;
	int phase;
};
static struct buzzer_callback_data_t bcd;

static void buzzer_callback(void * data, void * buf, int count)
{
	struct buzzer_callback_data_t * bcd = (struct buzzer_callback_data_t *)data;
	int16_t *p = buf;
	int i;

	count /= 2;
	for(i = 0; i < count; i++)
	{
		p[i] = bcd->amplitude * sin((2 * M_PI * (bcd->phase++) * bcd->frequency) / bcd->sample);
	}
}

void sandbox_sdl_buzzer_init(void)
{
	SDL_AudioSpec spec;

	bcd.amplitude = 0x7fff;
	bcd.frequency = 1000;
	bcd.sample = 48000;
	bcd.channel = 1;
	bcd.phase = 0;

	spec.freq = bcd.sample;
	spec.format = AUDIO_S16SYS;
	spec.channels = bcd.channel;
	spec.samples = 1024;
	spec.callback = (SDL_AudioCallback)buzzer_callback;
	spec.userdata = &bcd;

	SDL_LockAudio();
	SDL_OpenAudio(&spec, 0);
	SDL_UnlockAudio();
}

void sandbox_sdl_buzzer_exit(void)
{
	SDL_CloseAudio();
}

void sandbox_sdl_buzzer_set_frequency(int frequency)
{
	if(frequency == 0)
	{
		if(SDL_GetAudioStatus() != SDL_AUDIO_PAUSED)
			SDL_PauseAudio(1);
	}
	else
	{
		if(SDL_GetAudioStatus() != SDL_AUDIO_PLAYING)
			SDL_PauseAudio(0);
	}
	bcd.frequency = frequency;
}
