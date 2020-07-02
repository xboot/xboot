#ifndef __GNSS_H__
#define __GNSS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum gnss_signal_t {
	GNSS_SIGNAL_INVALID			= 0,
	GNSS_SIGNAL_FIX				= 1,
	GNSS_SIGNAL_DIFFERENTIAL	= 2,
	GNSS_SIGNAL_PPS				= 3,
	GNSS_SIGNAL_RTK				= 4,
	GNSS_SIGNAL_FLOAT_RTK		= 5,
	GNSS_SIGNAL_ESTIMATED		= 6,
	GNSS_SIGNAL_MANUAL			= 7,
	GNSS_SIGNAL_SIMULATION		= 8,
};

enum gnss_fix_t {
	GNSS_FIX_NONE				= 1,
	GNSS_FIX_2D					= 2,
	GNSS_FIX_3D					= 3,
};

struct gnss_nmea_t {
	enum gnss_signal_t signal;			/* signal quality */
	enum gnss_fix_t fix;				/* fix mode */
	unsigned int used;					/* the count of satellites in used */

	double latitude;					/* latitude in degrees */
	double longitude;					/* longitude in degrees */
	double altitude;					/* altitude in meters above the WGS-84 reference ellipsoid */
	double speed; 						/* speed over the ground in kilometers per hour */
	double track;						/* track angle in degrees true north */
	double mtrack;						/* magnetic track angle in degrees true north */
	double magvar;						/* magnetic variation */

	struct {
		unsigned int year;				/* year */
		unsigned int month;				/* month */
		unsigned int day;				/* day */
		unsigned int hour;				/* hour */
		unsigned int minute;			/* minute */
		unsigned int second;			/* second */
		unsigned int millisecond;		/* millisecond */
	} utc;

	struct {
		double pdop;					/* position dilution of precision */
		double hdop;					/* horizontal dilution of precision */
		double vdop;					/* vertical dilution of precision */
	} precision;

	struct {
		struct {
			unsigned int n;				/* the number of satellites */
			struct {
				unsigned int prn;		/* satellite prn number */
				unsigned int elevation;	/* elevation value in degrees */
				unsigned int azimuth;	/* azimuth value in degrees */
				unsigned int snr;		/* signal noise ratio in db */
			} sv[16];
		} gps;

		struct {
			unsigned int n;				/* the number of satellites */
			struct {
				unsigned int prn;		/* satellite prn number */
				unsigned int elevation;	/* elevation value in degrees */
				unsigned int azimuth;	/* azimuth value in degrees */
				unsigned int snr;		/* signal noise ratio in db */
			} sv[16];
		} beidou;
	} satellite;

	/*
	 * Private data
	 */
	char buffer[256];
	int bindex;
	int dindex;
	char * tstart[32];
	char * tend[32];
	int tsize;
};

struct gnss_t
{
	char * name;
	struct gnss_nmea_t nmea;

	void (*enable)(struct gnss_t * nav);
	void (*disable)(struct gnss_t * nav);
	int (*read)(struct gnss_t * nav, void * buf, int count);
	void * priv;
};

static inline struct gnss_nmea_t * gnss_nmea(struct gnss_t * nav)
{
	return &nav->nmea;
}

struct gnss_t * search_gnss(const char * name);
struct gnss_t * search_first_gnss(void);
struct device_t * register_gnss(struct gnss_t * nav, struct driver_t * drv);
void unregister_gnss(struct gnss_t * nav);

void gnss_enable(struct gnss_t * nav);
void gnss_disable(struct gnss_t * nav);
int gnss_refresh(struct gnss_t * nav);

#ifdef __cplusplus
}
#endif

#endif /* __GNSS_H__ */
