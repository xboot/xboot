#ifndef __BCM2837_MUART_H__
#define __BCM2837_MUART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <bus/uart.h>
#include <bcm2837/reg-aux.h>

struct bcm2837_muart_data_t
{
	char * clk;
	int txdpin;
	int txdcfg;
	int rxdpin;
	int rxdcfg;
	int baud;
	int data;
	int parity;
	int stop;
	physical_addr_t physaux;
	physical_addr_t physmu;
};

#ifdef __cplusplus
}
#endif

#endif /* __BCM2837_MUART_H__ */
