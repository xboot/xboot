#ifndef __DEFAULT_H__
#define __DEFAULT_H__

#include <configs.h>
#include <macros.h>


/**
 * default CONFIG_HEAP_SIZE is 8MB
 */
#if !defined(CONFIG_HEAP_SIZE)
#define	CONFIG_HEAP_SIZE							(SZ_8M)
#endif

/**
 * default CONFIG_MALLOC_CHUNK_SIZE is the minimum chunk size
 */
#if !defined(CONFIG_MALLOC_CHUNK_SIZE)
#define	CONFIG_MALLOC_CHUNK_SIZE					(SZ_64K)
#endif

/**
 * default CONFIG_PRINTK_BUF_SIZE is 4K
 */
#if !defined(CONFIG_PRINTK_BUF_SIZE)
#define	CONFIG_PRINTK_BUF_SIZE						(SZ_4K)
#endif

/**
 * the size of irq hash table
 */
#if !defined(CONFIG_IRQ_HASH_SIZE)
#define	CONFIG_IRQ_HASH_SIZE						(31)
#endif

/**
 * the size of env hash table
 */
#if !defined(CONFIG_ENV_HASH_SIZE)
#define	CONFIG_ENV_HASH_SIZE						(13)
#endif

/**
 * the size of clk hash table
 */
#if !defined(CONFIG_CLK_HASH_SIZE)
#define	CONFIG_CLK_HASH_SIZE						(1)
#endif

/**
 * the size of terminal hash table
 */
#if !defined(CONFIG_TERMINAL_HASH_SIZE)
#define	CONFIG_TERMINAL_HASH_SIZE					(1)
#endif

/**
 * the size of mmc host hash table
 */
#if !defined(CONFIG_MMC_HOST_HASH_SIZE)
#define	CONFIG_MMC_HOST_HASH_SIZE					(1)
#endif

/**
 * the size of readline buffer
 */
#if !defined(CONFIG_READLINE_BUF_SIZE)
#define	CONFIG_READLINE_BUF_SIZE					(SZ_4K)
#endif

/**
 * the max number of command line history
 */
#if !defined(CONFIG_MAX_NUMBER_OF_CMDLINE_HISTORY)
#define	CONFIG_MAX_NUMBER_OF_CMDLINE_HISTORY		(20)
#endif

/**
 * the max number of vfs bio
 */
#if !defined(CONFIG_MAX_NUMBER_OF_VFS_BIO)
#define	CONFIG_MAX_NUMBER_OF_VFS_BIO				(SZ_4K)
#endif


/**
 * the max number of log
 */
#if !defined(CONFIG_MAX_NUMBER_OF_LOG)
#define	CONFIG_MAX_NUMBER_OF_LOG					(1024)
#endif

/**
 * default CONFIG_CMDLINE_LENGTH is 4KB
 */
#if !defined(CONFIG_CMDLINE_LENGTH)
#define	CONFIG_CMDLINE_LENGTH						(SZ_4K)
#endif

/**
 * default CONFIG_VARNAME_LENGTH is 256
 */
#if !defined(CONFIG_VARNAME_LENGTH)
#define	CONFIG_VARNAME_LENGTH						(256)
#endif

/**
 * default CONFIG_HARDWARE_RTC_NAME is "rtc"
 */
#if !defined(CONFIG_HARDWARE_RTC_NAME)
#define	CONFIG_HARDWARE_RTC_NAME					("rtc")
#endif

/**
 * help command
 */
#if !defined(CONFIG_COMMAND_HELP)
#define	CONFIG_COMMAND_HELP							(1)
#endif

/**
 * echo command
 */
#if !defined(CONFIG_COMMAND_ECHO)
#define	CONFIG_COMMAND_ECHO							(1)
#endif

/*
 * env command
 */
#if !defined(CONFIG_COMMAND_ENV)
#define	CONFIG_COMMAND_ENV							(1)
#endif

/*
 * history command
 */
#if !defined(CONFIG_COMMAND_HISTORY)
#define	CONFIG_COMMAND_HISTORY						(1)
#endif

/*
 * version command
 */
#if !defined(CONFIG_COMMAND_VERSION)
#define	CONFIG_COMMAND_VERSION						(1)
#endif

/*
 * clear command
 */
#if !defined(CONFIG_COMMAND_CLEAR)
#define	CONFIG_COMMAND_CLEAR						(1)
#endif

/*
 * log command
 */
#if !defined(CONFIG_COMMAND_LOG)
#define	CONFIG_COMMAND_LOG							(1)
#endif

/*
 * tetris command
 */
#if !defined(CONFIG_COMMAND_TETRIS)
#define	CONFIG_COMMAND_TETRIS						(1)
#endif

/*
 * exit command
 */
#if !defined(CONFIG_COMMAND_EXIT)
#define	CONFIG_COMMAND_EXIT							(1)
#endif

/*
 * prompt command
 */
#if !defined(CONFIG_COMMAND_PROMPT)
#define	CONFIG_COMMAND_PROMPT						(1)
#endif

/*
 * nand command
 */
#if !defined(CONFIG_COMMAND_NAND)
#define	CONFIG_COMMAND_NAND							(1)
#endif

/*
 * sleep command
 */
#if !defined(CONFIG_COMMAND_SLEEP)
#define	CONFIG_COMMAND_SLEEP						(1)
#endif

/*
 * sync command
 */
#if !defined(CONFIG_COMMAND_SYNC)
#define	CONFIG_COMMAND_SYNC							(1)
#endif

/*
 * memtest command
 */
#if !defined(CONFIG_COMMAND_MEMTEST)
#define	CONFIG_COMMAND_MEMTEST						(1)
#endif

/*
 * go command
 */
#if !defined(CONFIG_COMMAND_GO)
#define	CONFIG_COMMAND_GO							(1)
#endif

/*
 * reboot command
 */
#if !defined(CONFIG_COMMAND_REBOOT)
#define	CONFIG_COMMAND_REBOOT						(1)
#endif

/*
 * xmodem command
 */
#if !defined(CONFIG_COMMAND_XMODEM)
#define	CONFIG_COMMAND_XMODEM						(1)
#endif

/*
 * halt command
 */
#if !defined(CONFIG_COMMAND_HALT)
#define	CONFIG_COMMAND_HALT							(1)
#endif

/*
 * serial command
 */
#if !defined(CONFIG_COMMAND_SERIAL)
#define	CONFIG_COMMAND_SERIAL						(1)
#endif

/*
 * mc command
 */
#if !defined(CONFIG_COMMAND_MC)
#define	CONFIG_COMMAND_MC							(1)
#endif

/*
 * md command
 */
#if !defined(CONFIG_COMMAND_MD)
#define	CONFIG_COMMAND_MD							(1)
#endif

/*
 * mw command
 */
#if !defined(CONFIG_COMMAND_MW)
#define	CONFIG_COMMAND_MW							(1)
#endif

/*
 * cksum command
 */
#if !defined(CONFIG_COMMAND_CKSUM)
#define	CONFIG_COMMAND_CKSUM						(1)
#endif

/*
 * load command
 */
#if !defined(CONFIG_COMMAND_LOAD)
#define	CONFIG_COMMAND_LOAD							(1)
#endif

/*
 * bootlinux command
 */
#if !defined(CONFIG_COMMAND_BOOTLINUX)
#define	CONFIG_COMMAND_BOOTLINUX					(1)
#endif

/*
 * dasm command
 */
#if !defined(CONFIG_COMMAND_DASM)
#define	CONFIG_COMMAND_DASM							(1)
#endif

/*
 * mount command
 */
#if !defined(CONFIG_COMMAND_MOUNT)
#define	CONFIG_COMMAND_MOUNT						(1)
#endif

/*
 * umount command
 */
#if !defined(CONFIG_COMMAND_UMOUNT)
#define	CONFIG_COMMAND_UMOUNT						(1)
#endif

/*
 * ls command
 */
#if !defined(CONFIG_COMMAND_LS)
#define	CONFIG_COMMAND_LS							(1)
#endif

/*
 * mkdir command
 */
#if !defined(CONFIG_COMMAND_MKDIR)
#define	CONFIG_COMMAND_MKDIR						(1)
#endif

/*
 * cd command
 */
#if !defined(CONFIG_COMMAND_CD)
#define	CONFIG_COMMAND_CD							(1)
#endif

/*
 * pwd command
 */
#if !defined(CONFIG_COMMAND_PWD)
#define	CONFIG_COMMAND_PWD							(1)
#endif

/*
 * rmdir command
 */
#if !defined(CONFIG_COMMAND_RMDIR)
#define	CONFIG_COMMAND_RMDIR						(1)
#endif

/*
 * rm command
 */
#if !defined(CONFIG_COMMAND_RM)
#define	CONFIG_COMMAND_RM							(1)
#endif

/*
 * mv command
 */
#if !defined(CONFIG_COMMAND_MV)
#define	CONFIG_COMMAND_MV							(1)
#endif

/*
 * fileram command
 */
#if !defined(CONFIG_COMMAND_FILERAM)
#define	CONFIG_COMMAND_FILERAM						(1)
#endif

/*
 * cp command
 */
#if !defined(CONFIG_COMMAND_CP)
#define	CONFIG_COMMAND_CP							(1)
#endif

/*
 * cat command
 */
#if !defined(CONFIG_COMMAND_CAT)
#define	CONFIG_COMMAND_CAT							(1)
#endif

/*
 * test command
 */
#if !defined(CONFIG_COMMAND_TEST)
#define	CONFIG_COMMAND_TEST							(1)
#endif

/*
 * nand ecc smart media byte order, default is no.
 */
#if !defined(CONFIG_MTD_NAND_ECC_SMC)
#define	CONFIG_MTD_NAND_ECC_SMC						(0)
#endif

/*
 * framebuffer terminal scroll up.
 */
#if !defined(CONFIG_FB_SCROLL_UP)
#define	CONFIG_FB_SCROLL_UP							(1)
#endif

/*
 * auto run delay in menu mode.
 */
#if !defined(CONFIG_MENU_MODE_AUTORUN_DELAY)
#define	CONFIG_MENU_MODE_AUTORUN_DELAY				(3)
#endif

#endif /* __DEFAULT_H__ */
