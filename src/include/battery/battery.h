#ifndef __BATTERY_H__
#define __BATTERY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum power_supply_t {
	POWER_SUPPLAY_BATTERY		= 0,
	POWER_SUPPLAY_AC			= 1,
	POWER_SUPPLAY_USB			= 2,
	POWER_SUPPLAY_WIRELESS		= 3,
};

enum battery_status_t {
	BATTERY_STATUS_DISCHARGING	= 0,
	BATTERY_STATUS_CHARGING		= 1,
	BATTERY_STATUS_EMPTY		= 2,
	BATTERY_STATUS_FULL			= 3,
};

enum battery_health_t {
	BATTERY_HEALTH_GOOD			= 0,
	BATTERY_HEALTH_DEAD			= 1,
	BATTERY_HEALTH_OVERHEAT		= 2,
	BATTERY_HEALTH_OVERVOLTAGE	= 3,
};

struct battery_info_t {
	enum power_supply_t supply;
	enum battery_status_t status;
	enum battery_health_t health;
	int design_capacity;	/* mAh */
	int design_voltage;		/* mV */
	int voltage;			/* mV */
	int current;			/* mA */
	int temperature;		/* 0.001°C */
	int cycle;				/* cycle count */
	int level;				/* Percentage */
};

struct battery_t
{
	char * name;

	bool_t (*update)(struct battery_t * bat, struct battery_info_t * info);
	void * priv;
};

struct battery_t * search_battery(const char * name);
struct battery_t * search_first_battery(void);
struct device_t * register_battery(struct battery_t * bat, struct driver_t * drv);
void unregister_battery(struct battery_t * bat);

char * power_supply_string(enum power_supply_t supply);
char * battery_status_string(enum battery_status_t status);
char * battery_health_string(enum battery_health_t health);
bool_t battery_update(struct battery_t * bat, struct battery_info_t * info);

#ifdef __cplusplus
}
#endif

#endif /* __BATTERY_H__ */
