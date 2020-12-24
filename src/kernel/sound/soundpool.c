/*
 * kernel/sound/soundpool.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <sound/soundpool.h>

static struct list_head __soundpool_list = {
	.next = &__soundpool_list,
	.prev = &__soundpool_list,
};
static spinlock_t __soundpool_lock = SPIN_LOCK_INIT();
static int __soundpool_vol = 4096;

static bool_t search_soundpool(struct sound_t * snd)
{
	struct sound_t * pos, * n;

	if(snd)
	{
		list_for_each_entry_safe(pos, n, &__soundpool_list, list)
		{
			if(pos == snd)
				return TRUE;
		}
	}
	return FALSE;
}

bool_t soundpool_add(struct sound_t * snd)
{
	irq_flags_t flags;

	if(!snd || search_soundpool(snd))
		return FALSE;

	spin_lock_irqsave(&__soundpool_lock, flags);
	list_add_tail(&snd->list, &__soundpool_list);
	spin_unlock_irqrestore(&__soundpool_lock, flags);
	return TRUE;
}

bool_t soundpool_remove(struct sound_t * snd)
{
	irq_flags_t flags;

	if(!snd || !search_soundpool(snd))
		return FALSE;

	spin_lock_irqsave(&__soundpool_lock, flags);
	list_del(&snd->list);
	spin_unlock_irqrestore(&__soundpool_lock, flags);
	return TRUE;
}

bool_t soundpool_clear(void (*cb)(struct sound_t *))
{
	struct sound_t * pos, * n;
	irq_flags_t flags;

	spin_lock_irqsave(&__soundpool_lock, flags);
	list_for_each_entry_safe(pos, n, &__soundpool_list, list)
	{
		list_del(&(pos->list));
		if(cb)
			cb(pos);
	}
	spin_unlock_irqrestore(&__soundpool_lock, flags);
	return TRUE;
}

int soundpool_get_volume(void)
{
	return __soundpool_vol;
}

void soundpool_set_volume(int vol)
{
	__soundpool_vol = clamp(vol, 0, 4096);
}

static int audio_playback_callback(void * data, void * buf, int count)
{
	struct audio_t * audio = (struct audio_t *)data;
	struct sound_t * pos, * n;
	irq_flags_t flags;
	char * pbuf = buf;
	int32_t left[480];
	int32_t right[480];
	int32_t result[480];
	int bytes = 0;
	int sample;
	int length;
	int nsound;
	int i;

	if(list_empty_careful(&__soundpool_list))
	{
		audio_playback_stop(audio);
		return 0;
	}
	while(count > 0)
	{
		sample = min((int)(count >> 2), 480);
		length = sample << 2;
		nsound = 0;
		memset(left, 0, length);
		memset(right, 0, length);
		spin_lock_irqsave(&__soundpool_lock, flags);
		list_for_each_entry_safe(pos, n, &__soundpool_list, list)
		{
			if(pos->loop != 0)
			{
				for(i = 0; i < sample; i++)
				{
					if(pos->sample > pos->postion)
					{
						int16_t * p = (int16_t *)(&pos->source[pos->postion]);
						left[i] += (p[0] * pos->lvol) >> 12;
						right[i] += (p[1] * pos->rvol) >> 12;
						pos->postion++;
					}
					else
					{
						if(pos->loop > 0)
							pos->loop--;
						if(pos->loop != 0)
						{
							pos->postion = 0;
							int16_t * p = (int16_t *)(&pos->source[pos->postion]);
							left[i] += (p[0] * pos->lvol) >> 12;
							right[i] += (p[1] * pos->rvol) >> 12;
						}
					}
				}
				nsound++;
			}
		}
		spin_unlock_irqrestore(&__soundpool_lock, flags);
		if(nsound > 0)
		{
			int16_t * p = (int16_t *)result;
			int32_t * pl = left;
			int32_t * pr = right;
			for(i = 0; i < sample; i++)
			{
				*p++ = clamp((pl[i] * __soundpool_vol / nsound) >> 12, -32768, 32767);
				*p++ = clamp((pr[i] * __soundpool_vol / nsound) >> 12, -32768, 32767);
			}
			memcpy(pbuf, result, length);
		}
		else
			memset(pbuf, 0, length);
		bytes += length;
		pbuf += length;
		count -= length;
	}
	return bytes;
}

void soundpool_playback(struct audio_t * audio)
{
	if(list_empty_careful(&__soundpool_list))
		return;
	audio_playback_start(audio, PCM_RATE_48000, PCM_FORMAT_BIT16, 2, audio_playback_callback, audio);
}
