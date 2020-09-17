#ifndef __X_H__
#define __X_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <libgen.h>
#include <termios.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <sys/timerfd.h>
#include <sys/inotify.h>
#include <sys/poll.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <linux/videodev2.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <alsa/asoundlib.h>
#include <gperftools/profiler.h>
#include <SDL.h>

#ifdef __cplusplus
}
#endif

#endif /* __X_H__ */
