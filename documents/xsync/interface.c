#include <interface.h>

uint64_t interface_time(struct interface_t * iface)
{
	if(iface && iface->time)
		return iface->time();
	return 0;
}

ssize_t interface_read(struct interface_t * iface, void * buf, size_t len)
{
	if(iface && iface->read)
		return iface->read(buf, len);
	return 0;
}

ssize_t interface_write(struct interface_t * iface, void * buf, size_t len)
{
	if(iface && iface->write)
		return iface->write(buf, len);
	return 0;
}
