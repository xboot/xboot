#ifndef __BCM2836_AUX_H__
#define __BCM2836_AUX_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	AUX_ID_MUART	= 0,
	AUX_ID_SPI1		= 1,
	AUX_ID_SPI2		= 2,
};

int bcm2836_aux_irq_status(int id);
void bcm2836_aux_enable(int id, int enable);

#ifdef __cplusplus
}
#endif

#endif /* __BCM2836_AUX_H__ */
