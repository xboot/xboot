#ifndef __REGULATOR_H__
#define __REGULATOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct regulator_t
{
	char * name;
	int count;

	void (*set_parent)(struct regulator_t * supply, const char * pname);
	const char * (*get_parent)(struct regulator_t * supply);
	void (*set_enable)(struct regulator_t * supply, bool_t enable);
	bool_t (*get_enable)(struct regulator_t * supply);
	void (*set_voltage)(struct regulator_t * supply, int voltage);
	int (*get_voltage)(struct regulator_t * supply);

	void * priv;
};

struct regulator_t * search_regulator(const char * name);
struct device_t * register_regulator(struct regulator_t * supply, struct driver_t * drv);
void unregister_regulator(struct regulator_t * supply);

void regulator_set_parent(const char * name, const char * pname);
const char * regulator_get_parent(const char * name);
void regulator_enable(const char * name);
void regulator_disable(const char * name);
bool_t regulator_status(const char * name);
void regulator_set_voltage(const char * name, int voltage);
int regulator_get_voltage(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __REGULATOR_H__ */

