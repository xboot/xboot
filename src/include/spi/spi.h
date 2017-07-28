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

	/* Activate chip select */
	void (*select)(struct spi_t * spi, int cs);

	/* Dectivate chip select */
	void (*deselect)(struct spi_t * spi, int cs);

	/* Private data */
	void * priv;
};

struct spi_device_t {
	struct spi_t * spi;
	int cs;
	int mode;
	int bits;
	int speed;
};

struct spi_t * search_spi(const char * name);
bool_t register_spi(struct device_t ** device, struct spi_t * spi);
bool_t unregister_spi(struct spi_t * spi);

int spi_transfer(struct spi_t * spi, struct spi_msg_t * msg);
void spi_select(struct spi_t * spi, int cs);
void spi_deselect(struct spi_t * spi, int cs);
struct spi_device_t * spi_device_alloc(const char * spibus, int cs, int mode, int bits, int speed);
void spi_device_free(struct spi_device_t * dev);
int spi_device_write_then_read(struct spi_device_t * dev, void * txbuf, int txlen, void * rxbuf, int rxlen);
void spi_device_select(struct spi_device_t * dev);
void spi_device_deselect(struct spi_device_t * dev);

#ifdef __cplusplus
}
#endif

#endif /* __BUS_SPI_H__ */
