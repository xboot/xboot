#include "usb_phy.h"
/* vbus, id, dpdm, these bit is set 1 to clear, so we clear these bit when operate other bits */
static unsigned int USBC_WakeUp_ClearChangeDetect(unsigned int reg_val)
{
	unsigned int temp = reg_val;

	temp &= ~(1 << USBC_BP_ISCR_VBUS_CHANGE_DETECT);
	temp &= ~(1 << USBC_BP_ISCR_ID_CHANGE_DETECT);
	temp &= ~(1 << USBC_BP_ISCR_DPDM_CHANGE_DETECT);

	return temp;
}
void USBC_EnableDpDmPullUp(void)
{
//	usbc_otg_t *usbc_otg = (usbc_otg_t *)hUSB;
	uint32_t reg_val = 0;

	/* vbus, id, dpdm, these bit is set 1 to clear, so we clear these bit when operate other bits */
	reg_val = USBC_Readl(USBC_REG_ISCR(USBC0_BASE));
	reg_val |= (1 << USBC_BP_ISCR_DPDM_PULLUP_EN);

	reg_val |= 3<<USBC_BP_ISCR_VBUS_VALID_SRC;

	reg_val = USBC_WakeUp_ClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(USBC0_BASE));


	//USBC_REG_set_bit_l(USBC_BP_ISCR_DPDM_PULLUP_EN,USBC_REG_ISCR(USBC0_BASE));//Pull up dp dm
}

void USBC_EnableIdPullUp(void)
{
	uint32_t reg_val = 0;

	/* vbus, id, dpdm, these bit is set 1 to clear, so we clear these bit when operate other bits */
	reg_val = USBC_Readl(USBC_REG_ISCR(USBC0_BASE));
	reg_val |= (1 << USBC_BP_ISCR_ID_PULLUP_EN);
	reg_val = USBC_WakeUp_ClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(USBC0_BASE));
}

static void USBC_ForceIdToLow(void )
{
	uint32_t reg_val = 0;
	/* first write 00, then write 10 */
	reg_val = USBC_Readl(USBC_REG_ISCR(USBC0_BASE));
	reg_val &= ~(0x03 << USBC_BP_ISCR_FORCE_ID);
	reg_val |= (0x02 << USBC_BP_ISCR_FORCE_ID);
	reg_val = USBC_WakeUp_ClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(USBC0_BASE));
}

static void USBC_ForceIdToHigh(void )
{
	uint32_t reg_val = 0;

	/* first write 00, then write 10 */
	reg_val = USBC_Readl(USBC_REG_ISCR(USBC0_BASE));
	//reg_val &= ~(0x03 << USBC_BP_ISCR_FORCE_ID);
	reg_val |= (0x03 << USBC_BP_ISCR_FORCE_ID);
	reg_val = USBC_WakeUp_ClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(USBC0_BASE));
}

static void USBC_ForceIdDisable(void)
{
	uint32_t reg_val = 0;

	/* vbus, id, dpdm, these bit is set 1 to clear, so we clear these bit when operate other bits */
	reg_val = USBC_Readl(USBC_REG_ISCR(USBC0_BASE));
	reg_val &= ~(0x03 << USBC_BP_ISCR_FORCE_ID);
	reg_val = USBC_WakeUp_ClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(USBC0_BASE));
}

/* force id to (id_type) */
void USBC_ForceId(uint32_t id_type)
{

	switch(id_type)
	{
		case USBC_ID_TYPE_HOST:
			USBC_ForceIdToLow();
			break;

		case USBC_ID_TYPE_DEVICE:
			USBC_ForceIdToHigh();
			break;

		default:
			USBC_ForceIdDisable();
	}
}

static void USBC_ForceVbusValidDisable(void )
{
	uint32_t reg_val = 0;

	/* first write 00, then write 10 */
	reg_val = USBC_Readl(USBC_REG_ISCR(USBC0_BASE));
	reg_val &= ~(0x03 << USBC_BP_ISCR_FORCE_VBUS_VALID);
	reg_val = USBC_WakeUp_ClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(USBC0_BASE));
}

static void USBC_ForceVbusValidToLow(void )
{
	uint32_t reg_val = 0;

	/* first write 00, then write 10 */
	reg_val = USBC_Readl(USBC_REG_ISCR(USBC0_BASE));
	reg_val &= ~(0x03 << USBC_BP_ISCR_FORCE_VBUS_VALID);
	reg_val |= (0x02 << USBC_BP_ISCR_FORCE_VBUS_VALID);
	reg_val = USBC_WakeUp_ClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(USBC0_BASE));
}

static void USBC_ForceVbusValidToHigh(void )
{
	uint32_t reg_val = 0;

	/* first write 00, then write 11 */
	reg_val = USBC_Readl(USBC_REG_ISCR(USBC0_BASE));
	//reg_val &= ~(0x03 << USBC_BP_ISCR_FORCE_VBUS_VALID);
	reg_val |= (0x03 << USBC_BP_ISCR_FORCE_VBUS_VALID);
	reg_val = USBC_WakeUp_ClearChangeDetect(reg_val);
	USBC_Writel(reg_val, USBC_REG_ISCR(USBC0_BASE));
}

/* force vbus valid to (id_type) */
void USBC_ForceVbusValid(uint32_t vbus_type)
{
	switch(vbus_type)
	{
	case USBC_VBUS_TYPE_LOW:
		USBC_ForceVbusValidToLow();
		break;

	case USBC_VBUS_TYPE_HIGH:
		USBC_ForceVbusValidToHigh();
		break;

	default:
		USBC_ForceVbusValidDisable();
	}
	return ;
}

/*
 * select the bus way for data transfer
 * @hUSB:     handle return by USBC_open_otg, include the key data which USBC need
 * @io_type:  bus type, pio or dma
 * @ep_type:  ep type, rx or tx
 * @ep_index: ep index
 *
 */
void USBC_SelectBus(uint32_t io_type, uint32_t ep_type, uint32_t ep_index)
{
	uint32_t reg_val = 0;

	reg_val = USBC_Readb(USBC_REG_VEND0(USBC0_BASE));
	if (io_type == USBC_IO_TYPE_DMA) {
		if (ep_type == USBC_EP_TYPE_TX) {
			reg_val |= ((ep_index - 0x01) << 1) << USBC_BP_VEND0_DRQ_SEL;  //drq_sel
			reg_val |= 0x1<<USBC_BP_VEND0_BUS_SEL;   //io_dma
		} else {
			reg_val |= ((ep_index << 1) - 0x01) << USBC_BP_VEND0_DRQ_SEL;
			reg_val |= 0x1<<USBC_BP_VEND0_BUS_SEL;
		}
	} else {
		//reg_val &= ~(0x1 << USBC_BP_VEND0_DRQ_SEL);  //clear drq_sel, select pio
		reg_val &= 0x00;  // clear drq_sel, select pio
	}

	/* in 1667 1673 and later ic, FIFO_BUS_SEL bit(bit24 of reg0x40 for host/device)
	 * is fixed to 1, the hw guarantee that it's ok for cpu/inner_dma/outer_dma transfer */

//	reg_val |= 0x1<<USBC_BP_VEND0_BUS_SEL;  //for 1663 set 1: enable dma, set 0: enable fifo

	USBC_Writeb(reg_val, USBC_REG_VEND0(USBC0_BASE));
}
static void usb_phy_write(int addr, int data, int len)
{
	int j = 0, usbc_bit = 0;
	void *dest = (void *)USBC_REG_CSR(USBC0_BASE);

//#ifdef CONFIG_MACH_SUN8I_A33
//	/* CSR needs to be explicitly initialized to 0 on A33 */
//	writel(0, dest);
//#endif

	usbc_bit = 1 << (0 * 2);
	for (j = 0; j < len; j++)
	{
		/* set the bit address to be written */
		USBC_ClrBit_Mask_l(dest, 0xff << 8);
		USBC_SetBit_Mask_l(dest, (addr + j) << 8);

		USBC_ClrBit_Mask_l(dest, usbc_bit);
		/* set data bit */
		if (data & 0x1)
			USBC_SetBit_Mask_l(dest, 1 << 7);
		else
			USBC_ClrBit_Mask_l(dest, 1 << 7);

		USBC_SetBit_Mask_l(dest, usbc_bit);

		USBC_ClrBit_Mask_l(dest, usbc_bit);

		data >>= 1;
	}
}
void USBC_PhyConfig(void)
{
	/* The following comments are machine
	 * translated from Chinese, you have been warned!
	 */

	/* Regulation 45 ohms */
	//if (phy->id == 0)
		usb_phy_write( 0x0c, 0x01, 1);

	/* adjust PHY's magnitude and rate */
	usb_phy_write(0x20, 0x14, 5);

	/* threshold adjustment disconnect */
#if defined CONFIG_MACH_SUN5I || defined CONFIG_MACH_SUN7I
	usb_phy_write(0x2a, 2, 2);
#else
	usb_phy_write( 0x2a, 3, 2);
#endif

	return;
}
void USBC_ConfigFIFO_Base(void)
{
	uint32_t reg_value;

	/* config usb fifo, 8kb mode */
	reg_value = USBC_Readl(SUNXI_SRAMC_BASE + 0x04);
	reg_value &= ~(0x03 << 0);
	reg_value |= (1 << 0);
	USBC_Writel(reg_value, SUNXI_SRAMC_BASE + 0x04);
}

uint32_t  usb_phy_open_clock(void)
{
	volatile int i;

	USBC_REG_set_bit_l(USBPHY_CLK_GAT_BIT, USBPHY_CLK_REG);
   USBC_REG_set_bit_l(USBPHY_CLK_RST_BIT, USBPHY_CLK_REG);

	USBC_REG_set_bit_l(BUS_CLK_USB_BIT,BUS_CLK_GATE0_REG);
	USBC_REG_set_bit_l(BUS_RST_USB_BIT,BUS_CLK_RST_REG);
    //USBC_Writel(0x0043031a,USBC_REG_PMU_IRQ(USBC0_BASE));
	return 0;
}
