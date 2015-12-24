#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/spi.h>
#include <block/block.h>

struct spi_flash_data_t
{
	const char * spibus;
};

#ifdef __cplusplus
}
#endif

#endif /* __SPI_FLASH_H__ */
