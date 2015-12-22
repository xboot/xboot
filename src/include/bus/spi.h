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
	int bits;
	int mode;
	int speed;
};

struct spi_t
{
	/* The spi bus name */
	char * name;

	/* Initialize spi */
	void (*init)(struct spi_t * spi);

	/* Clean up spi */
	void (*exit)(struct spi_t * spi);

	/* Spi master transfer */
	int (*transfer)(struct spi_t * spi, struct spi_msg_t * msgs);

	/* Spi Chip select */
	void (*chipselect)(struct spi_t * spi, int state);

	/* Private data */
	void * priv;
};

struct spi_t * search_bus_spi(const char * name);
bool_t register_bus_spi(struct spi_t * spi);
bool_t unregister_bus_spi(struct spi_t * spi);

#ifdef __cplusplus
}
#endif

#endif /* __BUS_SPI_H__ */
