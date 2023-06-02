#include <xboot.h>
#include <interrupt/interrupt.h>
#include "usb_phy.h"

extern void USBD_IRQHandler();
extern void USBH_IRQHandler();

void usb_dc_low_level_init(void)
{
    usb_phy_open_clock();
	USBC_PhyConfig();
	USBC_ConfigFIFO_Base();
	USBC_EnableDpDmPullUp();
	USBC_EnableIdPullUp();
	USBC_ForceId(USBC_ID_TYPE_DEVICE);
	USBC_ForceVbusValid( USBC_VBUS_TYPE_HIGH);

	request_irq(26, (void (*)(void *))USBD_IRQHandler, IRQ_TYPE_NONE, NULL);
}

void usb_hc_low_level_init(void)
{
    usb_phy_open_clock();
	USBC_PhyConfig();
	USBC_ConfigFIFO_Base();
	USBC_EnableDpDmPullUp();
	USBC_EnableIdPullUp();
	USBC_ForceId(USBC_ID_TYPE_HOST);
	USBC_ForceVbusValid( USBC_VBUS_TYPE_HIGH);

	request_irq(26, (void (*)(void *))USBD_IRQHandler, IRQ_TYPE_NONE, NULL);
}
