#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct adc_t
{
	char * name;
	int vreference;
	int resolution;
	int nchannel;

	u32_t (*read)(struct adc_t * adc, int channel);
	void * priv;
};

struct adc_t * search_adc(const char * name);
struct device_t * register_adc(struct adc_t * adc, struct driver_t * drv);
void unregister_adc(struct adc_t * adc);

u32_t adc_read_raw(struct adc_t * adc, int channel);
int adc_read_voltage(struct adc_t * adc, int channel);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
