#ifndef __XBOOT_CONFIGS_H__
#define __XBOOT_CONFIGS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <configs.h>
#include <endian.h>

#if !defined(__ARCH__)
#define __ARCH__									"arm32"
#endif

#if !defined(__MACH__)
#define __MACH__									"realview"
#endif

#if !defined(CONFIG_MMU_ON)
#define	CONFIG_MMU_ON								(0)
#endif

#if !defined(CONFIG_HEAP_SIZE)
#define	CONFIG_HEAP_SIZE							(SZ_8M)
#endif

#if !defined(CONFIG_NO_LOG)
#define	CONFIG_NO_LOG								(0)
#endif

#if !defined(CONFIG_MAX_BRIGHTNESS)
#define	CONFIG_MAX_BRIGHTNESS						(1023)
#endif

#if !defined(CONFIG_EVENT_FIFO_LENGTH)
#define	CONFIG_EVENT_FIFO_LENGTH					(256)
#endif

#if !defined(CONFIG_MAX_NUMBER_OF_VFS_BIO)
#define	CONFIG_MAX_NUMBER_OF_VFS_BIO				(SZ_4K)
#endif

#if !defined(CONFIG_CMDLINE_LENGTH)
#define	CONFIG_CMDLINE_LENGTH						(SZ_4K)
#endif

#if !defined(CONFIG_VARNAME_LENGTH)
#define	CONFIG_VARNAME_LENGTH						(256)
#endif

#if !defined(CONFIG_COMMAND_HELP)
#define	CONFIG_COMMAND_HELP							(1)
#endif

#if !defined(CONFIG_COMMAND_ECHO)
#define	CONFIG_COMMAND_ECHO							(1)
#endif

#if !defined(CONFIG_COMMAND_ENV)
#define	CONFIG_COMMAND_ENV							(1)
#endif

#if !defined(CONFIG_COMMAND_EXEC)
#define	CONFIG_COMMAND_EXEC							(1)
#endif

#if !defined(CONFIG_COMMAND_HISTORY)
#define	CONFIG_COMMAND_HISTORY						(1)
#endif

#if !defined(CONFIG_COMMAND_VERSION)
#define	CONFIG_COMMAND_VERSION						(1)
#endif

#if !defined(CONFIG_COMMAND_CLEAR)
#define	CONFIG_COMMAND_CLEAR						(1)
#endif

#if !defined(CONFIG_COMMAND_PROBE)
#define	CONFIG_COMMAND_PROBE						(1)
#endif

#if !defined(CONFIG_COMMAND_EXIT)
#define	CONFIG_COMMAND_EXIT							(1)
#endif

#if !defined(CONFIG_COMMAND_NAND)
#define	CONFIG_COMMAND_NAND							(1)
#endif

#if !defined(CONFIG_COMMAND_SLEEP)
#define	CONFIG_COMMAND_SLEEP						(1)
#endif

#if !defined(CONFIG_COMMAND_SYNC)
#define	CONFIG_COMMAND_SYNC							(1)
#endif

#if !defined(CONFIG_COMMAND_MEMTEST)
#define	CONFIG_COMMAND_MEMTEST						(1)
#endif

#if !defined(CONFIG_COMMAND_GO)
#define	CONFIG_COMMAND_GO							(1)
#endif

#if !defined(CONFIG_COMMAND_REBOOT)
#define	CONFIG_COMMAND_REBOOT						(1)
#endif

#if !defined(CONFIG_COMMAND_XMODEM)
#define	CONFIG_COMMAND_XMODEM						(1)
#endif

#if !defined(CONFIG_COMMAND_SHUTDOWN)
#define	CONFIG_COMMAND_SHUTDOWN						(1)
#endif

#if !defined(CONFIG_COMMAND_MC)
#define	CONFIG_COMMAND_MC							(1)
#endif

#if !defined(CONFIG_COMMAND_MD)
#define	CONFIG_COMMAND_MD							(1)
#endif

#if !defined(CONFIG_COMMAND_MW)
#define	CONFIG_COMMAND_MW							(1)
#endif

#if !defined(CONFIG_COMMAND_CKSUM)
#define	CONFIG_COMMAND_CKSUM						(1)
#endif

#if !defined(CONFIG_COMMAND_LOAD)
#define	CONFIG_COMMAND_LOAD							(1)
#endif

#if !defined(CONFIG_COMMAND_MOUNT)
#define	CONFIG_COMMAND_MOUNT						(1)
#endif

#if !defined(CONFIG_COMMAND_UMOUNT)
#define	CONFIG_COMMAND_UMOUNT						(1)
#endif

#if !defined(CONFIG_COMMAND_LS)
#define	CONFIG_COMMAND_LS							(1)
#endif

#if !defined(CONFIG_COMMAND_MKDIR)
#define	CONFIG_COMMAND_MKDIR						(1)
#endif

#if !defined(CONFIG_COMMAND_CD)
#define	CONFIG_COMMAND_CD							(1)
#endif

#if !defined(CONFIG_COMMAND_PWD)
#define	CONFIG_COMMAND_PWD							(1)
#endif

#if !defined(CONFIG_COMMAND_RMDIR)
#define	CONFIG_COMMAND_RMDIR						(1)
#endif

#if !defined(CONFIG_COMMAND_RM)
#define	CONFIG_COMMAND_RM							(1)
#endif

#if !defined(CONFIG_COMMAND_MV)
#define	CONFIG_COMMAND_MV							(1)
#endif

#if !defined(CONFIG_COMMAND_FILERAM)
#define	CONFIG_COMMAND_FILERAM						(1)
#endif

#if !defined(CONFIG_COMMAND_CP)
#define	CONFIG_COMMAND_CP							(1)
#endif

#if !defined(CONFIG_COMMAND_CAT)
#define	CONFIG_COMMAND_CAT							(1)
#endif

#if !defined(CONFIG_COMMAND_TEST)
#define	CONFIG_COMMAND_TEST							(1)
#endif

#if !defined(CONFIG_COMMAND_BOOTLINUX)
#define	CONFIG_COMMAND_BOOTLINUX					(1)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __XBOOT_CONFIGS_H__ */
