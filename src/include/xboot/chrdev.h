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
enum chrdev_type_t {
	CHR_DEV_SERIAL,
	CHR_DEV_FRAMEBUFFER,
	CHR_DEV_RTC,
	CHR_DEV_MISC,
};

/*
 * the struct of chrdev_t.
 */
struct chrdev_t
{
	/* the device name */
	const char * name;

	/* the type of char device */
	enum chrdev_type_t type;

	/* open device */
	int (*open)(struct chrdev_t * dev);

	/* read device */
	ssize_t (*read)(struct chrdev_t * dev, u8_t * buf, size_t count);

	/* write device */
	ssize_t (*write)(struct chrdev_t * dev, const u8_t * buf, size_t count);

	/* ioctl device */
	int (*ioctl)(struct chrdev_t * dev, int cmd, void * arg);

	/* close device */
	int (*close)(struct chrdev_t * dev);

	/* char device's driver */
	void * driver;
};


struct chrdev_t * search_chrdev(const char * name);
struct chrdev_t * search_chrdev_with_type(const char * name, enum chrdev_type_t type);
bool_t register_chrdev(struct chrdev_t * dev);
bool_t unregister_chrdev(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __CHRDEV_H__ */
