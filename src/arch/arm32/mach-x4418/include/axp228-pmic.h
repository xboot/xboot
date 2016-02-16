#ifndef __AXP228_PMIC_H__
#define __AXP228_PMIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/i2c.h>
#include <battery/battery.h>

struct axp228_pmic_data_t
{
	const char * i2cbus;
	u32_t addr;

	int dcdc1;
	int dcdc2;
	int dcdc3;
	int dcdc4;
	int dcdc5;
	int dc5ldo;

	int aldo1;
	int aldo2;
	int aldo3;

	int dldo1;
	int dldo2;
	int dldo3;
	int dldo4;

	int eldo1;
	int eldo2;
	int eldo3;
};

#ifdef __cplusplus
}
#endif

#endif /* __AXP228_PMIC_H__ */
