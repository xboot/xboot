#ifndef __CHRDEV_H__
#define __CHRDEV_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>


/* declare struct chrdev */
struct chrdev;

/*
 * the operations of char device
 */
struct char_operations
{
	/* open device */
	x_s32 (*open)(struct chrdev * dev);

	/* seek device */
	x_s32 (*seek)(struct chrdev * dev, x_s32 offset);

	/* read device */
	x_s32 (*read)(struct chrdev * dev, x_u8 * buf, x_s32 count);

	/* write device */
	x_s32 (*write)(struct chrdev * dev, const x_u8 * buf, x_s32 count);

	/* flush device */
	x_s32 (*flush)(struct chrdev * dev);

	/* ioctl device */
	x_s32 (*ioctl)(struct chrdev * dev, x_u32 cmd, x_u32 arg);

	/* release device */
	x_s32 (*release)(struct chrdev * dev);

	/* char device's driver */
	void * driver;
};

/*
 * the char device struct.
 */
struct chrdev
{
	/* the device name */
	char name[32+1];

	/* major device number */
	x_u32 major;

	/* minor device number */
	x_u32 minor;

	/* the operations of char device */
	struct char_operations * ops;
};

/*
 * the list of chrdev list
 */
struct chrdev_list
{
	struct chrdev * dev;
	struct hlist_node node;
};


inline x_u32 chrdev_major_to_index(x_u32 major);
struct chrdev * search_chrdev_by_major_name(x_u32 major, const char *name);
struct chrdev * search_chrdev_by_major_minor(x_u32 major, x_u32 minor);
x_bool register_chrdev(x_u32 major, const char *name, const struct char_operations *ops);
x_bool unregister_chrdev(x_u32 major, const char *name);


#endif /* __CHRDEV_H__ */
