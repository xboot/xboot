#ifndef __SDHCI_XL00500_H__
#define __SDHCI_XL00500_H__

#ifdef __cplusplus
extern "C" {
#endif

#define VERBOSE

struct sdmmc_register_set
{
	// Internal Register
	volatile u32_t    CTRL;                       ///< 0x000 : Control Register                               [  31:   0]
	volatile u32_t    PWREN;                      ///< 0x004 : Power Enable Register                          [  63:  32]
	volatile u32_t    CLKDIV;                     ///< 0x008 : Clock Divider Register                         [  95:  64]
	volatile u32_t    CLKSRC;                     ///< 0x00C : Clock Source Register                          [ 127:  96]
	volatile u32_t    CLKENA;                     ///< 0x010 : Clock Enable Register                          [ 159: 128]
	volatile u32_t    TMOUT;                      ///< 0x014 : Time-Out Register                              [ 191: 160]
	volatile u32_t    CTYPE;                      ///< 0x018 : Card Type Register                             [ 223: 192]
	volatile u32_t    BLKSIZ;                     ///< 0x01C : Block Size Register                            [ 255: 224]
	volatile u32_t    BYTCNT;                     ///< 0x020 : Byte Count Register                            [ 287: 256]
	volatile u32_t    INTMASK;                    ///< 0x024 : Interrupt Mask Register                        [ 319: 288]
	volatile u32_t    CMDARG;                     ///< 0x028 : Command Argument Register                      [ 351: 320]
	volatile u32_t    CMD;                        ///< 0x02C : Command Register                               [ 383: 352]
	volatile u32_t    RESP0;                      ///< 0x030 : Response 0 Register                            [ 415: 384]
	volatile u32_t    RESP1;                      ///< 0x034 : Response 1 Register                            [ 447: 416]
	volatile u32_t    RESP2;                      ///< 0x038 : Response 2 Register                            [ 479: 448]
	volatile u32_t    RESP3;                      ///< 0x03C : Response 3 Register                            [ 511: 480]
	volatile u32_t    MINTSTS;                    ///< 0x040 : Masked Interrupt Status Register               [ 543: 512]
	volatile u32_t    RINTSTS;                    ///< 0x044 : Raw Interrupt Status Register                  [ 575: 544]
	volatile u32_t    STATUS;                     ///< 0x048 : Status Register - Mainly for Debug Purpose     [ 607: 576]
	volatile u32_t    FIFOTH;                     ///< 0x04C : FIFO Threshold Register                        [ 639: 608]
	volatile u32_t    CDETECT;                    ///< 0x050 : Card Detect Register                           [ 671: 640]
	volatile u32_t    WRTPRT;                     ///< 0x054 : Write Protect Register                         [ 703: 672]
	volatile u32_t    GPIO;                       ///< 0x058 : General Purpose Input/Output Register          [ 735: 704]
	volatile u32_t    TCBCNT;                     ///< 0x05C : Transferred CIU card byte count                [ 767: 736]
	volatile u32_t    TBBCNT;                     ///< 0x060 : Transferred Host Byte Count                    [ 799: 768]
	volatile u32_t    DEBNCE;                     ///< 0x064 : Card Detect Debounce Register                  [ 831: 800]
	volatile u32_t    USRID;                      ///< 0x068 : User ID Register                               [ 863: 832]
	volatile u32_t    VERID;                      ///< 0x06C : Version ID Register                            [ 895: 864]
	volatile u32_t    HCON;                       ///< 0x070 : Hardware Configuration Register                [ 927: 896]
	volatile u32_t    UHS_REG;                    ///< 0x074 : UHS_REG register
	volatile u32_t    RSTn;                       ///< 0x078 : Hardware reset register
	volatile u32_t    _Reserved0;                 ///< 0x07C
	volatile u32_t    BMOD;                       ///< 0x080 : Bus Mode Register
	volatile u32_t    PLDMND;                     ///< 0x084 : Poll Demand Register
	volatile u32_t    DBADDR;                     ///< 0x088 : Descriptor List Base Address Register
	volatile u32_t    IDSTS;                      ///< 0x08C : Internal DMAC Status Register
	volatile u32_t    IDINTEN;                    ///< 0x090 : Internal DMAC Interrupt Enable Register
	volatile u32_t    DSCADDR;                    ///< 0x094 : Current Host Descriptor Address Register
	volatile u32_t    BUFADDR;                    ///< 0x098 : Current Buffer Descriptor Address Register
	volatile u8_t     _Reserved1[0x100-0x09C];    ///< 0x09C ~ 0x100 reserved area
	volatile u32_t    CARDTHRCTL;                 ///< 0x100 : Card Read Threshold Enable
	volatile u32_t    BACKEND_POWER;              ///< 0x104 : Back-end Power
	volatile u32_t    UHS_REG_EXT;                ///< 0x108 : eMMC 4.5 1.2V Register
	volatile u32_t    EMMC_DDR_REG;               ///< 0x10C : eMMC DDR START bit detection control register
	volatile u32_t    ENABLE_SHIFT;               ///< 0x110 : Phase shift control register
	volatile u32_t    CLKCTRL;                    ///< 0x114 : External clock phase & delay control register
	volatile u8_t     _Reserved2[0x200-0x118];    ///< 0x118 ~ 0x200
	volatile u32_t    DATA;                       ///< 0x200 : Data
};

#if (0)
#define INFINTE_LOOP()					{ while(1); }
#else
#define INFINTE_LOOP()
#endif

#define	SDMMC_STATUS_NOERROR			0
#define	SDMMC_STATUS_ERROR				(1U<<31)
#define	SDMMC_STATUS_CMDBUSY			(SDMMC_STATUS_ERROR | (1U<<0))
#define	SDMMC_STATUS_CMDTOUT			(SDMMC_STATUS_ERROR | (1U<<1))
#define	SDMMC_STATUS_RESCRCFAIL			(SDMMC_STATUS_ERROR | (1U<<2))
#define	SDMMC_STATUS_RESERROR			(SDMMC_STATUS_ERROR | (1U<<3))
#define	SDMMC_STATUS_RESTOUT			(SDMMC_STATUS_ERROR | (1U<<4))
#define SDMMC_STATUS_UNKNOWNCMD			(SDMMC_STATUS_ERROR | (1U<<5))
#define	SDMMC_STATUS_DATABUSY			(SDMMC_STATUS_ERROR | (1U<<6))


#define SDXC_CTRL_USEINDMAC 			(1U<<25)
#define SDXC_CTRL_READWAIT	 			(1U<< 6)
#define SDXC_CTRL_DMAMODE_EN			(1U<< 5)
#define SDXC_CTRL_DMARST				(1U<< 2)
#define SDXC_CTRL_FIFORST				(1U<< 1)
#define SDXC_CTRL_CTRLRST				(1U<< 0)

//--------------------------------------------------------------------------
#define SDXC_CLKENA_LOWPWR				(1U<<16)
#define SDXC_CLKENA_CLKENB				(1U<< 0)

//--------------------------------------------------------------------------
#define SDXC_STATUS_FIFOCOUNT			(0x1FFFU<<17)
#define SDXC_STATUS_FSMBUSY				(1U<<10)
#define SDXC_STATUS_DATABUSY			(1U<< 9)
#define SDXC_STATUS_DATA3BUSY			(1U<< 8)
#define SDXC_STATUS_FIFOFULL			(1U<< 3)
#define SDXC_STATUS_FIFOEMPTY			(1U<< 2)

//--------------------------------------------------------------------------
#define SDXC_CMDFLAG_STARTCMD			(1U<<31)
#define SDXC_CMDFLAG_USE_HOLD_REG		(1U<<29)
#define SDXC_CMDFLAG_VOLT_SWITCH		(1U<<28)
#define SDXC_CMDFLAG_BOOT_MODE			(1U<<27)
#define SDXC_CMDFLAG_DISABLE_BOOT		(1U<<26)
#define SDXC_CMDFLAG_EXPECTBOOTACK		(1U<<25)
#define SDXC_CMDFLAG_ENABLE_BOOT		(1U<<24)
#define SDXC_CMDFLAG_CCS_EXPECTED		(1U<<23)
#define SDXC_CMDFLAG_READCEATADEVICE	(1U<<22)
#define SDXC_CMDFLAG_UPDATECLKONLY		(1U<<21)
#define SDXC_CMDFLAG_SENDINIT			(1U<<15)
#define SDXC_CMDFLAG_STOPABORT			(1U<<14)
#define SDXC_CMDFLAG_WAITPRVDAT			(1U<<13)
#define SDXC_CMDFLAG_SENDAUTOSTOP		(1U<<12)
#define SDXC_CMDFLAG_BLOCK				(0U<<11)
#define SDXC_CMDFLAG_STREAM				(1U<<11)
#define SDXC_CMDFLAG_TXDATA				(3U<< 9)
#define SDXC_CMDFLAG_RXDATA				(1U<< 9)
#define SDXC_CMDFLAG_CHKRSPCRC			(1U<< 8)
#define SDXC_CMDFLAG_SHORTRSP			(1U<< 6)
#define SDXC_CMDFLAG_LONGRSP			(3U<< 6)

//--------------------------------------------------------------------------
#define SDXC_RINTSTS_SDIO				(1U<<16)
#define SDXC_RINTSTS_EBE				(1U<<15)
#define SDXC_RINTSTS_ACD				(1U<<14)
#define SDXC_RINTSTS_SBE				(1U<<13)
#define SDXC_RINTSTS_HLE				(1U<<12)
#define SDXC_RINTSTS_FRUN				(1U<<11)
#define SDXC_RINTSTS_HTO				(1U<<10)
#define SDXC_RINTSTS_DRTO				(1U<< 9)
#define SDXC_RINTSTS_RTO				(1U<< 8)
#define SDXC_RINTSTS_DCRC				(1U<< 7)
#define SDXC_RINTSTS_RCRC				(1U<< 6)
#define SDXC_RINTSTS_RXDR				(1U<< 5)
#define SDXC_RINTSTS_TXDR				(1U<< 4)
#define SDXC_RINTSTS_DTO				(1U<< 3)
#define SDXC_RINTSTS_CD					(1U<< 2)
#define SDXC_RINTSTS_RE					(1U<< 1)

#define BLOCK_LENGTH					(512)
#define SDMMC_TIMEOUT					(0x100000)
#define SDMMC_TIMEOUT_IDENTIFY			(0x100000)

#define CLKSRC_PLL_0     (0)
#define CLKSRC_PLL_1     (1)
#define CLKSRC_PLL_2     (2)
#define CLKSRC_PLL_3     (3)


//------------------------------------------------------------------------------
#define CLKSRC_UART      (CLKSRC_PLL_0)
#define CLKSRC_SDMMC     (CLKSRC_PLL_2)
#define CLKSRC_SPI       (CLKSRC_PLL_0)

//------------------------------------------------------------------------------
// Reset Controller : Number of Reset
//------------------------------------------------------------------------------
#define NUMBER_OF_RESET_MODULE_PIN 69
enum {
	// xl50200_AC97_cfg0
	RESETINDEX_OF_AC97_MODULE_PRESETn = 0
	,// nx01301_CORTEXA9MP_TOP_QUADL2C
	RESETINDEX_OF_nx01301_CORTEXA9MP_TOP_QUADL2C_MODULE_nCPURESET1 = 1
	,// nx01301_CORTEXA9MP_TOP_QUADL2C
	RESETINDEX_OF_nx01301_CORTEXA9MP_TOP_QUADL2C_MODULE_nCPURESET2 = 2
	,// nx01301_CORTEXA9MP_TOP_QUADL2C
	RESETINDEX_OF_nx01301_CORTEXA9MP_TOP_QUADL2C_MODULE_nCPURESET3 = 3
	,// nx01301_CORTEXA9MP_TOP_QUADL2C
	RESETINDEX_OF_nx01301_CORTEXA9MP_TOP_QUADL2C_MODULE_nWDRESET1 = 4
	,// nx01301_CORTEXA9MP_TOP_QUADL2C
	RESETINDEX_OF_nx01301_CORTEXA9MP_TOP_QUADL2C_MODULE_nWDRESET2 = 5
	,// nx01301_CORTEXA9MP_TOP_QUADL2C
	RESETINDEX_OF_nx01301_CORTEXA9MP_TOP_QUADL2C_MODULE_nWDRESET3 = 6
	,// nx02600_CRYPTO_cfg0
	RESETINDEX_OF_CRYPTO_MODULE_i_nRST = 7
	,// nx01501_DEINTERLACE_cfg0
	RESETINDEX_OF_DEINTERLACE_MODULE_i_nRST = 8
	,// nx71000_DisplayTop_cfg5
	RESETINDEX_OF_DISPLAYTOP_MODULE_i_Top_nRST = 9
	,// nx71000_DisplayTop_cfg5
	RESETINDEX_OF_DISPLAYTOP_MODULE_i_DualDisplay_nRST = 10
	,// nx71000_DisplayTop_cfg5
	RESETINDEX_OF_DISPLAYTOP_MODULE_i_ResConv_nRST = 11
	,// nx71000_DisplayTop_cfg5
	RESETINDEX_OF_DISPLAYTOP_MODULE_i_LCDIF_nRST = 12
	,// nx71000_DisplayTop_cfg5
	RESETINDEX_OF_DISPLAYTOP_MODULE_i_HDMI_nRST = 13
	,// nx71000_DisplayTop_cfg5
	RESETINDEX_OF_DISPLAYTOP_MODULE_i_HDMI_VIDEO_nRST = 14
	,// nx71000_DisplayTop_cfg5
	RESETINDEX_OF_DISPLAYTOP_MODULE_i_HDMI_SPDIF_nRST = 15
	,// nx71000_DisplayTop_cfg5
	RESETINDEX_OF_DISPLAYTOP_MODULE_i_HDMI_TMDS_nRST = 16
	,// nx71000_DisplayTop_cfg5
	RESETINDEX_OF_DISPLAYTOP_MODULE_i_HDMI_PHY_nRST = 17
	,// nx71000_DisplayTop_cfg5
	RESETINDEX_OF_DISPLAYTOP_MODULE_i_LVDS_nRST = 18
	,// nx50100_ECID_128bit
	RESETINDEX_OF_ECID_MODULE_i_nRST = 19
	,// xl00100_I2C_cfg0
	RESETINDEX_OF_I2C0_MODULE_PRESETn = 20
	,// xl00100_I2C_cfg0
	RESETINDEX_OF_I2C1_MODULE_PRESETn = 21
	,// xl00100_I2C_cfg0
	RESETINDEX_OF_I2C2_MODULE_PRESETn = 22
	,// xl00300_I2S_cfg1
	RESETINDEX_OF_I2S0_MODULE_PRESETn = 23
	,// xl00300_I2S_cfg1
	RESETINDEX_OF_I2S1_MODULE_PRESETn = 24
	,// xl00300_I2S_cfg1
	RESETINDEX_OF_I2S2_MODULE_PRESETn = 25
	,// xl00112_DREX_cfg1
	RESETINDEX_OF_DREX_MODULE_CRESETn = 26
	,// xl00112_DREX_cfg1
	RESETINDEX_OF_DREX_MODULE_ARESETn = 27
	,// xl00112_DREX_cfg1
	RESETINDEX_OF_DREX_MODULE_nPRST = 28
	,// nx71100_mipi_cfg1
	RESETINDEX_OF_MIPI_MODULE_i_nRST = 29
	,// nx71100_mipi_cfg1
	RESETINDEX_OF_MIPI_MODULE_i_DSI_I_PRESETn = 30
	,// nx71100_mipi_cfg1
	RESETINDEX_OF_MIPI_MODULE_i_CSI_I_PRESETn = 31
	,// nx71100_mipi_cfg1
	RESETINDEX_OF_MIPI_MODULE_i_PHY_S_RESETN = 32
	,// nx71100_mipi_cfg1
	RESETINDEX_OF_MIPI_MODULE_i_PHY_M_RESETN = 33
	,// nx01910_MPEGTSI_cfg0
	RESETINDEX_OF_MPEGTSI_MODULE_i_nRST = 34
	,// nx02500_PDM_cfg0
	RESETINDEX_OF_PDM_MODULE_i_nRST = 35
	,// xl50010_PWMTIMER_usetimer
	RESETINDEX_OF_TIMER_MODULE_PRESETn = 36
	,// xl50010_PWMTIMER_usepwm
	RESETINDEX_OF_PWM_MODULE_PRESETn = 37
	,// nx01400_SCALER_cfg0
	RESETINDEX_OF_SCALER_MODULE_i_nRST = 38
	,// xl00500_SDMMC_cfg0
	RESETINDEX_OF_SDMMC0_MODULE_i_nRST = 39
	,// xl00500_SDMMC_cfg0
	RESETINDEX_OF_SDMMC1_MODULE_i_nRST = 40
	,// xl00500_SDMMC_cfg0
	RESETINDEX_OF_SDMMC2_MODULE_i_nRST = 41
	,// nx01600_SPDIFRX_cfg0
	RESETINDEX_OF_SPDIFRX_MODULE_PRESETn = 42
	,// xl50300_SPDIFTX_hdmipinout
	RESETINDEX_OF_SPDIFTX_MODULE_PRESETn = 43
	,// pl02212_Ssp_cfg1
	RESETINDEX_OF_SSP0_MODULE_PRESETn = 44
	,// pl02212_Ssp_cfg1
	RESETINDEX_OF_SSP0_MODULE_nSSPRST = 45
	,// pl02212_Ssp_cfg1
	RESETINDEX_OF_SSP1_MODULE_PRESETn = 46
	,// pl02212_Ssp_cfg1
	RESETINDEX_OF_SSP1_MODULE_nSSPRST = 47
	,// pl02212_Ssp_cfg1
	RESETINDEX_OF_SSP2_MODULE_PRESETn = 48
	,// pl02212_Ssp_cfg1
	RESETINDEX_OF_SSP2_MODULE_nSSPRST = 49
	,// pl01115_Uart_cfg0
	RESETINDEX_OF_UART0_MODULE_nUARTRST = 50
	,// pl01115_Uart_modem
	RESETINDEX_OF_pl01115_Uart_modem_MODULE_nUARTRST = 51
	,// pl01115_Uart_cfg0
	RESETINDEX_OF_UART1_MODULE_nUARTRST = 52
	,// pl01115_Uart_nodma
	RESETINDEX_OF_pl01115_Uart_nodma0_MODULE_nUARTRST = 53
	,// pl01115_Uart_nodma
	RESETINDEX_OF_pl01115_Uart_nodma1_MODULE_nUARTRST = 54
	,// pl01115_Uart_nodma
	RESETINDEX_OF_pl01115_Uart_nodma2_MODULE_nUARTRST = 55
	,// xl00700_USB20HOST_cfg0
	RESETINDEX_OF_USB20HOST_MODULE_i_nRST = 56
	,// xl00600_USB20OTG_cfg0
	RESETINDEX_OF_USB20OTG_MODULE_i_nRST = 57
	,// xl50500_WDT_cfg0
	RESETINDEX_OF_WDT_MODULE_PRESETn = 58
	,// xl50500_WDT_cfg0
	RESETINDEX_OF_WDT_MODULE_nPOR = 59
	,// nx01700_adc_cfg0
	RESETINDEX_OF_ADC_MODULE_nRST = 60
	,// xl07000_coda960_cfg1
	RESETINDEX_OF_CODA960_MODULE_i_areset_n = 61
	,// xl07000_coda960_cfg1
	RESETINDEX_OF_CODA960_MODULE_i_preset_n = 62
	,// xl07000_coda960_cfg1
	RESETINDEX_OF_CODA960_MODULE_i_creset_n = 63
	,// xl50400_DWC_GMAC_RGMII
	RESETINDEX_OF_DWC_GMAC_MODULE_aresetn_i = 64
	,// xl06000_mali400_cfg1
	RESETINDEX_OF_MALI400_MODULE_nRST = 65
	,// nx02300_ppm_cfg0
	RESETINDEX_OF_PPM_MODULE_i_nRST = 66
#if 0
	,// nx01800_vip_twopadwrapper
	RESETINDEX_OF_VIP1_MODULE_i_nRST = 67
	,// nx01800_vip_cfg3
	RESETINDEX_OF_VIP0_MODULE_i_nRST = 68
#else
	,// nx01800_vip_twopadwrapper
	RESETINDEX_OF_VIP1_MODULE_i_nRST = 68
	,// nx01800_vip_cfg3
	RESETINDEX_OF_VIP0_MODULE_i_nRST = 67
#endif
};

///	@brief	type for PCLK control mode
typedef enum
{
	PCLKMODE_DYNAMIC = 0UL,		///< PCLK is provided only when CPU has access to registers of this module.
	PCLKMODE_ALWAYS	= 1UL		///< PCLK is always provided for this module.
} PCLKMODE ;

///	@brief type for BCLK control mode
typedef enum
{
	BCLKMODE_DISABLE	= 0UL,	///< BCLK is disabled.
	BCLKMODE_DYNAMIC	= 2UL,	///< BCLK is provided only when this module requests it.
	BCLKMODE_ALWAYS	= 3UL		///< BCLK is always provided for this module.
} BCLKMODE ;

struct clkgen_register_set
{
	volatile u32_t	CLKENB;			// Clock Enable Register
	volatile u32_t	CLKGEN;			// Clock Generate Register
};

typedef struct {
	u32_t pllnum;
    u32_t freq;
    u32_t clkdiv;
    u32_t clkgendiv;
} clkinfo;

#define PLL_P   18
#define PLL_M   8
#define PLL_S   0
#define PLL_K   16


#define PHY_BASEADDR_RSTCON_MODULE		0xC0012000
#define SetIO32(Addr,Data)      (*(volatile unsigned int  *)(Addr)) |=  ((unsigned int  )(Data))
#define ClearIO32(Addr,Data)    (*(volatile unsigned int  *)(Addr)) &= ~((unsigned int  )(Data))


#define PHY_BASEADDR_CLKGEN18_MODULE		0xC00C5000
#define PHY_BASEADDR_CLKGEN19_MODULE		0xC00CC000
#define PHY_BASEADDR_CLKGEN20_MODULE		0xC00CD000

struct rstcon_register_set
{
	volatile u32_t	REGRST[1];			//
};

#ifdef __cplusplus
}
#endif

#endif /* __SDHCI_XL00500_H__ */
