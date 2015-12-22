#ifndef __SPI_ALGO_BIT_H__
#define __SPI_ALGO_BIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/spi.h>

struct spi_algo_bit_data_t {
	void (*setclk)(struct spi_algo_bit_data_t * data, int state);
	void (*setmosi)(struct spi_algo_bit_data_t * data, int state);
	int (*getmiso)(struct spi_algo_bit_data_t * data);
	void (*setcs)(struct spi_algo_bit_data_t * data, int state);
	void * priv;
};

int spi_algo_bit_xfer(struct spi_algo_bit_data_t * bdat, struct spi_msg_t * msgs, int num);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_ALGO_BIT_H__ */
