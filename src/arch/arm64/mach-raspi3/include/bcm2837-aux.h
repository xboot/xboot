#ifndef __BCM2837_AUX_H__
#define __BCM2837_AUX_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	AUX_ID_UART	= 0,
	AUX_ID_SPI1	= 1,
	AUX_ID_SPI2	= 2,
};

void bcm2837_aux_enable(int id);
void bcm2837_aux_disable(int id);
int bcm2837_aux_irq_status(int id);

#ifdef __cplusplus
}
#endif

#endif /* __BCM2837_AUX_H__ */
