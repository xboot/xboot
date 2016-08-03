#ifndef __BUS_SPI_H__
#define __BUS_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct spi_msg_t {
	void * txbuf;
	void * rxbuf;
	int len;
	int mode;
	int bits;
	int speed;
};

struct spi_t
{
	/* The spi bus name */
	char * name;

	/* Master transfer */
	int (*transfer)(struct spi_t * spi, struct spi_msg_t * msgs);

	/* Chip select */
	void (*chipselect)(struct spi_t * spi, int state);

	/* Private data */
	void * priv;
};

struct spi_device_t {
	struct spi_t * spi;
	int mode;
	int bits;
	int speed;
};

struct spi_t * search_spi(const char * name);
bool_t register_spi(struct device_t ** device, struct spi_t * spi);
bool_t unregister_spi(struct spi_t * spi);

int spi_transfer(struct spi_t * spi, struct spi_msg_t * msg);
void spi_chipselect(struct spi_t * spi, int enable);
struct spi_device_t * spi_device_alloc(const char * spibus, int mode, int bits, int speed);
void spi_device_free(struct spi_device_t * dev);
int spi_device_write_then_read(struct spi_device_t * dev, void * txbuf, int txlen, void * rxbuf, int rxlen);
void spi_device_chipselect(struct spi_device_t * dev, int enable);

#ifdef __cplusplus
}
#endif

#endif /* __BUS_SPI_H__ */
