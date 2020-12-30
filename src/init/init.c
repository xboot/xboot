/*
 * init/init.c
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
#include <init.h>

void do_show_logo(void)
{
	struct device_t * pos, * n;
	struct xfs_context_t * ctx;
	struct surface_t * s, * logo;
	struct framebuffer_t * fb;
	struct matrix_t m;
	struct color_t c;
	char key[256];
	int brightness;

	if(!list_empty_careful(&__device_head[DEVICE_TYPE_FRAMEBUFFER]))
	{
		ctx = xfs_alloc("/private/framework", 0);
		if(ctx)
		{
			logo = surface_alloc_from_xfs(ctx, "assets/images/logo.png");
			if(logo)
			{
				list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_FRAMEBUFFER], head)
				{
					if((fb = (struct framebuffer_t *)(pos->priv)))
					{
						s = framebuffer_create_surface(fb);
						matrix_init_identity(&m);
						color_init(&c, 0x33, 0x99, 0xcc, 0xff);
						surface_fill(s, NULL, &m, surface_get_width(s), surface_get_height(s), &c, RENDER_TYPE_GOOD);
						matrix_init_translate(&m, (surface_get_width(s) - surface_get_width(logo)) / 2, (surface_get_height(s) - surface_get_height(logo)) / 2);
						surface_blit(s, NULL, &m, logo, RENDER_TYPE_GOOD);
						framebuffer_present_surface(fb, s, NULL);
						framebuffer_destroy_surface(fb, s);
						sprintf(key, "backlight(%s)", fb->name);
						brightness = strtol(setting_get(key, "-1"), NULL, 0);
						if(brightness <= 0)
							brightness = (1000 * 633) >> 10;
						framebuffer_set_backlight(fb, brightness);
					}
				}
				surface_free(logo);
			}
			xfs_free(ctx);
		}
	}
}

void do_play_audio(void)
{
	struct device_t * pos, * n;
	struct xfs_context_t * ctx;
	struct sound_t * snd;
	struct audio_t * audio;
	char key[256];
	int volume;

	if(!list_empty_careful(&__device_head[DEVICE_TYPE_AUDIO]))
	{
		ctx = xfs_alloc("/private/framework", 0);
		if(ctx)
		{
			list_for_each_entry_safe(pos, n, &__device_head[DEVICE_TYPE_AUDIO], head)
			{
				if((audio = (struct audio_t *)(pos->priv)))
				{
					if(audio->playback_start && audio->playback_stop)
					{
						sprintf(key, "playback-volume(%s)", audio->name);
						volume = strtol(setting_get(key, "-1"), NULL, 0);
						if(volume <= 0)
							volume = (1000 * 633) >> 10;
						audio_ioctl(audio, "audio-set-playback-volume", &volume);
						snd = sound_alloc_from_xfs(ctx, "assets/sounds/boot.wav");
						if(snd)
						{
							sound_set_callback(snd, sound_free);
							audio_playback(audio, snd);
						}
					}
				}
			}
			xfs_free(ctx);
		}
	}
}

static int nm_call(struct notifier_t * n, int cmd, void * arg)
{
	struct device_t * dev = (struct device_t *)arg;
	struct filesystem_t * fpos, * fn;
	struct vfs_stat_t st;
	struct vfs_mount_t * m;
	char fpath[VFS_MAX_PATH];
	int i, cnt;

	if(dev && (dev->type == DEVICE_TYPE_BLOCK))
	{
		switch(cmd)
		{
		case NOTIFIER_DEVICE_ADD:
			cnt = vfs_mount_count();
			for(i = 0; i < cnt; i++)
			{
				m = vfs_mount_get(i);
				if(m->m_dev == (struct block_t *)dev->priv)
					return 0;
			}
			sprintf(fpath, "/storage/%s", dev->name);
			if(vfs_stat(fpath, &st) < 0)
				vfs_mkdir(fpath, 0755);
			list_for_each_entry_safe(fpos, fn, &__filesystem_list, list)
			{
				if(vfs_mount(dev->name, fpath, fpos->name, MOUNT_RW) == 0)
					return 0;
			}
			vfs_rmdir(fpath);
			break;
		case NOTIFIER_DEVICE_REMOVE:
			cnt = vfs_mount_count();
			for(i = 0; i < cnt; i++)
			{
				m = vfs_mount_get(i);
				if(m->m_dev == (struct block_t *)dev->priv)
				{
					vfs_force_unmount(m);
					break;
				}
			}
			sprintf(fpath, "/storage/%s", dev->name);
			if(vfs_stat(fpath, &st) == 0 && S_ISDIR(st.st_mode))
				vfs_rmdir(fpath);
			break;
		case NOTIFIER_DEVICE_SUSPEND:
			break;
		case NOTIFIER_DEVICE_RESUME:
			break;
		default:
			break;
		}
	}
	return 0;
}

static struct notifier_t nm = {
	.priority	= 100,
	.call		= nm_call,
};

void do_auto_mount(void)
{
	struct filesystem_t * fpos, * fn;
	struct device_t * dpos, * dn;
	struct vfs_stat_t st;
	struct vfs_mount_t * m;
	char fpath[VFS_MAX_PATH];
	int i, cnt, found, mounted;

	list_for_each_entry_safe(dpos, dn, &__device_head[DEVICE_TYPE_BLOCK], head)
	{
		cnt = vfs_mount_count();
		for(i = 0, found = 0; i < cnt; i++)
		{
			m = vfs_mount_get(i);
			if(m->m_dev == (struct block_t *)dpos->priv)
			{
				found = 1;
				break;
			}
		}
		if(!found)
		{
			sprintf(fpath, "/storage/%s", dpos->name);
			if(vfs_stat(fpath, &st) < 0)
				vfs_mkdir(fpath, 0755);
			mounted = 0;
			list_for_each_entry_safe(fpos, fn, &__filesystem_list, list)
			{
				if(vfs_mount(dpos->name, fpath, fpos->name, MOUNT_RW) == 0)
				{
					mounted = 1;
					break;
				}
			}
			if(!mounted)
				vfs_rmdir(fpath);
		}
	}
	register_device_notifier(&nm);
}

void do_auto_boot(void)
{
	int delay = CONFIG_AUTO_BOOT_DELAY * 1000;

	while(delay > 0)
	{
		if(getchar() != EOF)
		{
			printf("\r\n");
			return;
		}
		mdelay(10);
		delay -= 10;
		if(delay < 0)
			delay = 0;
		printf("\rPress any key to stop auto boot:%3d.%03d%s", delay / 1000, delay % 1000, (delay == 0) ? "\r\n" : "");
	}

#ifdef __SANDBOX__
	extern char * sandbox_get_application(void);
	if(sandbox_get_application())
		shell_system(sandbox_get_application());
	else
		shell_system(CONFIG_AUTO_BOOT_COMMAND);
#else
	shell_system(CONFIG_AUTO_BOOT_COMMAND);
#endif
}
