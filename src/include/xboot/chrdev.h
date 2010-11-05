#ifndef __CHRDEV_H__
#define __CHRDEV_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>


/*
 * char device type
 */
enum chrdev_type {
	CHR_DEV_SERIAL,
	CHR_DEV_FRAMEBUFFER,
	CHR_DEV_RTC,
	CHR_DEV_MISC,
};

/*
 * the char device struct.
 */
struct chrdev
{
	/* the device name */
	const char * name;

	/* the type of char device */
	enum chrdev_type type;

	/* open device */
	x_s32 (*open)(struct chrdev * dev);

	/* read device */
	x_s32 (*read)(struct chrdev * dev, x_u8 * buf, x_s32 count);

	/* write device */
	x_s32 (*write)(struct chrdev * dev, const x_u8 * buf, x_s32 count);

	/* ioctl device */
	x_s32 (*ioctl)(struct chrdev * dev, x_u32 cmd, void * arg);

	/* close device */
	x_s32 (*close)(struct chrdev * dev);

	/* char device's driver */
	void * driver;
};


struct chrdev * search_chrdev(const char * name);
struct chrdev * search_chrdev_with_type(const char * name, enum chrdev_type type);
x_bool register_chrdev(struct chrdev * dev);
x_bool unregister_chrdev(const char * name);

#endif /* __CHRDEV_H__ */
