#ifndef __SPI_GPIO_H__
#define __SPI_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/spi.h>

struct spi_gpio_data_t {
	int sclk_pin;
	int mosi_pin;
	int miso_pin;
	int cs_pin;
};

#ifdef __cplusplus
}
#endif

#endif /* __SPI_GPIO_H__ */
