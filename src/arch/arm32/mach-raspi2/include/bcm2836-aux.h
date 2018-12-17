#ifndef __BCM2836_AUX_H__
#define __BCM2836_AUX_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	AUX_ID_UART	= 0,
	AUX_ID_SPI1	= 1,
	AUX_ID_SPI2	= 2,
};

void bcm2836_aux_enable(int id);
void bcm2836_aux_disable(int id);
int bcm2836_aux_irq_status(int id);

#ifdef __cplusplus
}
#endif

#endif /* __BCM2836_AUX_H__ */
