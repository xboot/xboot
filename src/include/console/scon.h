#ifndef __SERIAL_CONSOLE_H__
#define __SERIAL_CONSOLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <serial/serial.h>

bool_t register_serial_console(struct serial_driver_t * serial);
bool_t unregister_serial_console(struct serial_driver_t * serial);

#ifdef __cplusplus
}
#endif

#endif /* __SERIAL_CONSOLE_H__ */
