#ifndef __CHRDEV_H__
#define __CHRDEV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
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
	int (*open)(struct chrdev * dev);

	/* read device */
	ssize_t (*read)(struct chrdev * dev, u8_t * buf, size_t count);

	/* write device */
	ssize_t (*write)(struct chrdev * dev, const u8_t * buf, size_t count);

	/* ioctl device */
	int (*ioctl)(struct chrdev * dev, int cmd, void * arg);

	/* close device */
	int (*close)(struct chrdev * dev);

	/* char device's driver */
	void * driver;
};


struct chrdev * search_chrdev(const char * name);
struct chrdev * search_chrdev_with_type(const char * name, enum chrdev_type type);
bool_t register_chrdev(struct chrdev * dev);
bool_t unregister_chrdev(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __CHRDEV_H__ */
