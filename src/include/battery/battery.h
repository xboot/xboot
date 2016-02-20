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
	BATTERY_STATUS_FULL			= 2,
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
	int count;			/* Charge count */
	int capacity;		/* mAh */
	int voltage;		/* mV */
	int current;		/* mA */
	int temperature;	/* 0.1 Centigrade */
	int level;			/* Percentage */
};

struct battery_t
{
	/* The battery name */
	char * name;

	/* Initialize the battery */
	void (*init)(struct battery_t * bat);

	/* Clean up the battery */
	void (*exit)(struct battery_t * bat);

	/*  Battery update ... */
	bool_t (*update)(struct battery_t * bat, struct battery_info_t * info);

	/* Suspend battery */
	void (*suspend)(struct battery_t * bat);

	/* Resume battery */
	void (*resume)(struct battery_t * bat);

	/* Private data */
	void * priv;
};

struct battery_t * search_battery(const char * name);
bool_t register_battery(struct battery_t * bat);
bool_t unregister_battery(struct battery_t * bat);
bool_t battery_update(struct battery_t * bat, struct battery_info_t * info);

#ifdef __cplusplus
}
#endif

#endif /* __BATTERY_H__ */
