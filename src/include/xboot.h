#ifndef __XBOOT_H__
#define __XBOOT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xconfigs.h>
#include <endian.h>
#include <atomic.h>
#include <barrier.h>
#include <irqflags.h>
#include <spinlock.h>
#include <types.h>
#include <sizes.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <ctype.h>
#include <errno.h>
#include <environ.h>
#include <byteorder.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <locale.h>
#include <time.h>
#include <math.h>
#include <exit.h>
#include <io.h>
#include <ssize.h>
#include <malloc.h>
#include <charset.h>
#include <version.h>
#include <runtime.h>
#include <xboot/list.h>
#include <xboot/initcall.h>
#include <xboot/kobj.h>
#include <xboot/kref.h>
#include <xboot/proc.h>
#include <xboot/resource.h>
#include <xboot/irq.h>
#include <xboot/machine.h>
#include <xboot/bus.h>
#include <xboot/device.h>
#include <xboot/module.h>
#include <xboot/event.h>
#include <logger/logger.h>
#include <gpio/gpio.h>
#include <clk/clk.h>
#include <clocksource/clocksource.h>
#include <time/tick.h>
#include <time/timer.h>
#include <fs/fileio.h>
#include <mode/mode.h>

#ifdef __cplusplus
}
#endif

#endif /* __XBOOT_H__ */
