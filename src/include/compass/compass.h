#ifndef __COMPASS_H__
#define __COMPASS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct compass_t
{
	char * name;
	int ox, oy, oz;
	bool_t (*get)(struct compass_t * c, int * x, int * y, int * z);
	void * priv;
};

struct compass_t * search_compass(const char * name);
struct compass_t * search_first_compass(void);
bool_t register_compass(struct device_t ** device,struct compass_t * c);
bool_t unregister_compass(struct compass_t * c);

bool_t compass_set_offset(struct compass_t * c, int ox, int oy, int oz);
bool_t compass_get_offset(struct compass_t * c, int * ox, int * oy, int * oz);
bool_t compass_get_magnetic(struct compass_t * c, int * x, int * y, int * z);
bool_t compass_get_heading(struct compass_t * c, int * heading, int declination);

#ifdef __cplusplus
}
#endif

#endif /* __COMPASS_H__ */
