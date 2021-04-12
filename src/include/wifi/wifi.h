#ifndef __WIFI_H__
#define __WIFI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct wifi_t
{
	char * name;

	bool_t (*join)(struct wifi_t * wifi, const char * ssid, const char * passwd);
	bool_t (*quit)(struct wifi_t * wifi);
	bool_t (*connect)(struct wifi_t * wifi, const char * ip, int port);
	bool_t (*disconnect)(struct wifi_t * wifi);
	bool_t (*status)(struct wifi_t * wifi);
	int (*read)(struct wifi_t * wifi, void * buf, int count);
	int (*write)(struct wifi_t * wifi, void * buf, int count);
	int (*ioctl)(struct wifi_t * wifi, const char * cmd, void * arg);

	void * priv;
};

struct wifi_t * search_wifi(const char * name);
struct wifi_t * search_first_wifi(void);
struct device_t * register_wifi(struct wifi_t * wifi, struct driver_t * drv);
void unregister_wifi(struct wifi_t * wifi);

bool_t wifi_join(struct wifi_t * wifi, const char * ssid, const char * passwd);
bool_t wifi_quit(struct wifi_t * wifi);
bool_t wifi_connect(struct wifi_t * wifi, const char * ip, int port);
bool_t wifi_disconnect(struct wifi_t * wifi);
bool_t wifi_status(struct wifi_t * wifi);
int wifi_read(struct wifi_t * wifi, void * buf, int count);
int wifi_write(struct wifi_t * wifi, void * buf, int count);
int wifi_ioctl(struct wifi_t * wifi, const char * cmd, void * arg);

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H__ */
