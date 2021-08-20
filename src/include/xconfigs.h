#ifndef __XBOOT_CONFIGS_H__
#define __XBOOT_CONFIGS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <configs.h>

#if !defined(CONFIG_NO_LOG)
#define CONFIG_NO_LOG						(0)
#endif

#if !defined(CONFIG_MAX_SMP_CPUS)
#define CONFIG_MAX_SMP_CPUS					(1)
#endif

#if !defined(CONFIG_TASK_STACK_SIZE)
#define CONFIG_TASK_STACK_SIZE				(512 * 1024)
#endif

#if !defined(CONFIG_DRIVER_HASH_SIZE)
#define CONFIG_DRIVER_HASH_SIZE				(521)
#endif

#if !defined(CONFIG_DEVICE_HASH_SIZE)
#define CONFIG_DEVICE_HASH_SIZE				(521)
#endif

#if !defined(CONFIG_EVENT_FIFO_SIZE)
#define CONFIG_EVENT_FIFO_SIZE				(64)
#endif

#if !defined(CONFIG_MOUNT_PRIVATE_DEVICE)
#define CONFIG_MOUNT_PRIVATE_DEVICE			""
#endif

#if !defined(CONFIG_SHELL_TASK)
#define CONFIG_SHELL_TASK					"shell"
#endif

#if !defined(CONFIG_AUTO_BOOT_DELAY)
#define CONFIG_AUTO_BOOT_DELAY				(1)
#endif

#if !defined(CONFIG_AUTO_BOOT_COMMAND)
#define CONFIG_AUTO_BOOT_COMMAND			"launcher"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __XBOOT_CONFIGS_H__ */
