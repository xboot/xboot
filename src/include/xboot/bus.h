#ifndef __BUS_H__
#define __BUS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum bus_type_t {
	BUS_TYPE_1WIRE,
	BUS_TYPE_UART,
	BUS_TYPE_IIC,
	BUS_TYPE_SPI,
	BUS_TYPE_CAN,
	BUS_TYPE_USB,
};

struct bus_t
{
	/* Bus name */
	char * name;

	/* Bus type */
	enum bus_type_t type;

	/* Bus driver */
	void * driver;
};

struct bus_list_t
{
	struct bus_t * bus;
	struct list_head entry;
};

extern struct bus_list_t __bus_list;

struct bus_t * search_bus(const char * name);
struct bus_t * search_bus_with_type(const char * name, enum bus_type_t type);
bool_t register_bus(struct bus_t * bus);
bool_t unregister_bus(struct bus_t * bus);

#ifdef __cplusplus
}
#endif

#endif /* __BUS_H__ */
