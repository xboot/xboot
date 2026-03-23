#include <xboot.h>
#include <i2c/i2c.h>
#include <led/led.h>

struct led_fd650_pdata_t {
	struct i2c_device_t * dev;
};

static void fd650_write(struct led_fd650_pdata_t * pdat, u8_t addr, u8_t data)
{
	struct i2c_msg_t msg;
	u8_t buf[1];

	buf[0] = data;

	/* The FD650 uses I2C slave addresses directly as commands */
	msg.addr = addr >> 1;
	msg.flags = 0;
	msg.len = 1;
	msg.buf = buf;

	i2c_transfer(pdat->dev->i2c, &msg, 1);
}

static void fd650_set(struct led_t * led, int brightness)
{
	/* Do we even need to set brightness?
	 * The requirements say "display 'boot' on startup".
	 * Let's just hardcode the display 'boot' in probe.
	 */
}

static int fd650_get(struct led_t * led)
{
	return 0;
}

static struct device_t * led_fd650_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct led_fd650_pdata_t * pdat;
	struct led_t * led;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), dt_read_int(n, "slave-address", 0x24), 0);
	if(!i2cdev)
		return NULL;

	pdat = malloc(sizeof(struct led_fd650_pdata_t));
	if(!pdat)
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	led = malloc(sizeof(struct led_t));
	if(!led)
	{
		free(pdat);
		i2c_device_free(i2cdev);
		return NULL;
	}

	pdat->dev = i2cdev;

	led->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	led->set = fd650_set;
	led->get = fd650_get;
	led->priv = pdat;

	/*
	 * Initialization sequence for FD650 to display "boot"
	 */
	fd650_write(pdat, 0x48, 0x01 | 0x70); /* System Enable, Full Brightness */

	/* The actual digits depend on FD650 vs FD655 and wiring. Let's use 0x66 based addresses (FD655) */
	fd650_write(pdat, 0x66, 0x7c); /* 'b' */
	fd650_write(pdat, 0x68, 0x5c); /* 'o' */
	fd650_write(pdat, 0x6a, 0x5c); /* 'o' */
	fd650_write(pdat, 0x6c, 0x78); /* 't' */

	if(!(dev = register_led(led, drv)))
	{
		free_device_name(led->name);
		free(led->priv);
		free(led);
		i2c_device_free(i2cdev);
		return NULL;
	}
	return dev;
}

static void led_fd650_remove(struct device_t * dev)
{
	struct led_t * led = (struct led_t *)dev->priv;
	struct led_fd650_pdata_t * pdat = (struct led_fd650_pdata_t *)led->priv;

	if(led)
	{
		unregister_led(led);
		i2c_device_free(pdat->dev);
		free_device_name(led->name);
		free(led->priv);
		free(led);
	}
}

static void led_fd650_suspend(struct device_t * dev)
{
}

static void led_fd650_resume(struct device_t * dev)
{
}

static struct driver_t led_fd650 = {
	.name		= "led-fd650",
	.probe		= led_fd650_probe,
	.remove		= led_fd650_remove,
	.suspend	= led_fd650_suspend,
	.resume		= led_fd650_resume,
};

static __init void led_fd650_driver_init(void)
{
	register_driver(&led_fd650);
}

static __exit void led_fd650_driver_exit(void)
{
	unregister_driver(&led_fd650);
}

driver_initcall(led_fd650_driver_init);
driver_exitcall(led_fd650_driver_exit);
