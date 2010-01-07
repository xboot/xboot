#ifndef __MAJOR_H__
#define __MAJOR_H__

/*
 * definitions for major device numbers.
 */
#define	MAJOR_UNNAMED			(0)

/*
 * major for char device.
 */
#define MAJOR_SERIAL			(1)
#define	MAJOR_KEYBOARD			(2)
#define	MAJOR_MOUSE				(3)
#define	MAJOR_TOUCHSCREEN		(4)
#define	MAJOR_JOYSTICK			(5)
#define	MAJOR_FRAMEBUFFER		(6)
#define MAJOR_RTC				(7)
#define MAJOR_MISC				(8)

#define MAJOR_CHAR_MAX			(255)


#endif /* __MAJOR_H__ */
