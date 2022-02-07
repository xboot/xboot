/* SPDX-License-Identifier: MIT */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef __GX6605S_PLATFORM_H__
#define __GX6605S_PLATFORM_H__

#include <gx6605s.h>

enum gx6605s_gctl_registers {
    GX6605S_MPEG_CLD_RST0_DAT       = 0x00,
    GX6605S_MPEG_CLD_RST0_SET       = 0x04,
    GX6605S_MPEG_CLD_RST0_CLR       = 0x08,

    GX6605S_MPEG_HOT_RST0_DAT       = 0x0c,
    GX6605S_MPEG_HOT_RST0_SET       = 0x10,
    GX6605S_MPEG_HOT_RST0_CLR       = 0x14,

    GX6605S_MPEG_CLK_INHIBIT_DAT    = 0x18,
    GX6605S_MPEG_CLK_INHIBIT_SET    = 0x1c,
    GX6605S_MPEG_CLK_INHIBIT_CLR    = 0x20,
    GX6605S_CLOCK_DIV_CONFIG0       = 0x24,

    GX6605S_DTO0_CONFIG	            = 0x28,     /* Audio I2S        */
    GX6605S_DTO1_CONFIG	            = 0x2c,     /* Audio Spdif      */
    GX6605S_DTO2_CONFIG	            = 0x30,     /* Video            */
    GX6605S_DTO3_CONFIG	            = 0x34,     /* JPEG             */
    GX6605S_DTO4_CONFIG	            = 0x38,     /* PP               */
    GX6605S_DTO5_CONFIG	            = 0x3c,     /* Audio Decoder    */
    GX6605S_DTO6_CONFIG	            = 0x40,     /* GA               */
    GX6605S_DTO7_CONFIG	            = 0x44,     /* Demux SYS        */
    GX6605S_DTO8_CONFIG	            = 0x48,     /* Demux STC        */
    GX6605S_DTO9_CONFIG             = 0x4c,     /* APB              */
    GX6605S_DTO10_CONFIG            = 0x50,     /* UART             */
    GX6605S_DTO11_CONFIG            = 0x54,     /* CPU              */
    GX6605S_DTO12_CONFIG            = 0x58,
    GX6605S_DTO13_CONFIG            = 0x5c,
    GX6605S_DTO14_CONFIG            = 0x60,
    GX6605S_DTO15_CONFIG            = 0x64,

    GX6605S_MPEG_CLK_INHIBIT2_NORM  = 0x68,
    GX6605S_MPEG_CLK_INHIBIT2_1SET  = 0x6c,
    GX6605S_MPEG_CLK_INHIBIT2_1CLR  = 0x70,

    GX6605S_CPU_BASE                = 0x0c0,
    GX6605S_PLL_DTO_BASE            = 0x0c8,
    GX6605S_PLL_DVB_BASE            = 0x0d0,
    GX6605S_PLL_DDR_BASE            = 0x0e0,

    GX6605S_USB_CONFIG              = 0x100,
    GX6605S_USB0_CONFIG             = 0x104,
    GX6605S_USB1_CONFIG             = 0x108,
    GX6605S_USB2_CONFIG             = 0x10c,

    GX6605S_DVB_CONFIG              = 0x110,
    GX6605S_EPHY_CONFIG             = 0x114,

    GX6605S_DRAM_CTRL0              = 0x120,
    GX6605S_DRAM_CTRL1              = 0x124,
    GX6605S_DRAM_CTRL2              = 0x128,

    GX6605S_PINMUX_PORTA            = 0x13C,
    GX6605S_PINMUX_PORTB            = 0x140,
    GX6605S_PINMUX_PORTC            = 0x144,
    GX6605S_PINMUX_PORTD            = 0x148,

    GX6605S_MMU_CONGIG              = 0x164,

    GX6605S_SOURCE_SEL0             = 0x170,
    GX6605S_SOURCE_SEL1             = 0x174,
    GX6605S_CLOCK_DIV_CONFIG1       = 0x178,
    GX6605S_CLOCK_DIV_CONFIG2       = 0x17C,

    GX6605S_AUDIO_CODEC_DATA        = 0x1A0,
    GX6605S_AUDIO_CODEC_CONTROL     = 0x1A4,

    GX6605S_MPEG_CLD_RST1_DAT       = 0x1c0,
    GX6605S_MPEG_CLD_RST1_SET       = 0x1c4,
    GX6605S_MPEG_CLD_RST1_CLR       = 0x1c8,

    GX6605S_MPEG_HOT_RST1_DAT       = 0x1d0,
    GX6605S_MPEG_HOT_RST1_SET       = 0x1d4,
    GX6605S_MPEG_HOT_RST1_CLR       = 0x1d8,

    GX6605S_HDMI_REG_DATA           = 0x1E0,
    GX6605S_HDMI_REG_CONTROL        = 0x1E4,
    GX6605S_HDMI_KEY                = 0x1E8,
    GX6605S_DAC                     = 0x1F0,
    GX6605S_ADC                     = 0x200,

    GX6605S_STATE_ADC               = 0x204,

    GX6605S_DENALI_CONFIG0          = 0x580,
    GX6605S_CPU_32BIT_ANLIGN        = 0x600,
};

enum gx6605s_rcc_registers {
    GX6605S_RCC_CTRL_REG            = 0x00,
    GX6605S_RCC_STA_REG             = 0x04,
    GX6605S_RCC_INTERVAL            = 0x08,
    GX6605S_RCC_START_ADDR0         = 0x10,
    GX6605S_RCC_SIZE0               = 0x14,
    GX6605S_RCC_START_ADDR1         = 0x18,
    GX6605S_RCC_SIZE1               = 0x1c,
    GX6605S_RCC_START_ADDR2         = 0x20,
    GX6605S_RCC_SIZE2               = 0x24,
    GX6605S_RCC_START_ADDR3         = 0x28,
    GX6605S_RCC_SIZE3               = 0x2c,
    GX6605S_HASH_START_ADDR         = 0x40,
    GX6605S_HASH_SIZE               = 0x44,
    GX6605S_RCC_HASH_A              = 0x50,
    GX6605S_RCC_HASH_B              = 0x54,
    GX6605S_RCC_HASH_C              = 0x58,
    GX6605S_RCC_HASH_D              = 0x5c,
    GX6605S_RCC_HASH_E              = 0x60,
    GX6605S_RCC_HASH_F              = 0x64,
    GX6605S_RCC_HASH_G              = 0x68,
    GX6605S_RCC_HASH_H              = 0x6c,
};

enum gx6605s_pinmux {
    GX6605S_GPIO0_DBG_TDI           = 0,
    GX6605S_GPIO0_PORT0             = 1,
    GX6605S_GPIO0_ADCO_DATA0        = 2,

    GX6605S_GPIO1_DBG_TDO           = 0,
    GX6605S_GPIO1_PORT1             = 1,
    GX6605S_GPIO1_ADCO_DATA1        = 2,

    GX6605S_GPIO2_DBG_TMS           = 0,
    GX6605S_GPIO2_PORT2             = 1,
    GX6605S_GPIO2_ADCO_DATA2        = 2,

    GX6605S_GPIO3_DBG_TCK           = 0,
    GX6605S_GPIO3_PORT3             = 1,
    GX6605S_GPIO3_ADCO_DATA3        = 2,

    GX6605S_GPIO4_DBG_TRST          = 0,
    GX6605S_GPIO4_PORT4             = 1,
    GX6605S_GPIO4_ADCO_DATA4        = 2,

    GX6605S_GPIO5_SC1_CLK           = 0,
    GX6605S_GPIO5_PORT5             = 1,
    GX6605S_GPIO5_TSI1_DATA0        = 2,
    GX6605S_GPIO5_AJ_TDI            = 3,
    GX6605S_GPIO5_DVBFSYNC          = 4,
    GX6605S_GPIO5_ADCO_DATA5        = 5,

    GX6605S_GPIO6_SC1_RST           = 0,
    GX6605S_GPIO6_PORT6             = 1,
    GX6605S_GPIO6_TSI1_DATA1        = 2,
    GX6605S_GPIO6_AJ_TDO            = 3,
    GX6605S_GPIO6_ADCO_DATA6        = 4,

    GX6605S_GPIO7_SC1_PWR           = 0,
    GX6605S_GPIO7_PORT7             = 1,
    GX6605S_GPIO7_TSI1_DATA2        = 2,
    GX6605S_GPIO7_AJ_TMS            = 3,
    GX6605S_GPIO7_ADCO_DATA7        = 4,

    GX6605S_GPIO8_SC1_CD            = 0,
    GX6605S_GPIO8_PORT8             = 1,
    GX6605S_GPIO8_TSI1_DATA3        = 2,
    GX6605S_GPIO8_AJ_TCK            = 3,
    GX6605S_GPIO8_ADCO_DATA8        = 4,

    GX6605S_GPIO9_SC1_DATA          = 0,
    GX6605S_GPIO9_PORT9             = 1,
    GX6605S_GPIO9_TSI1_DATA4        = 2,
    GX6605S_GPIO9_AJ_RST            = 3,
    GX6605S_GPIO9_ADCO_DATA9        = 4,

    GX6605S_GPIO10_DISEQCI          = 0,
    GX6605S_GPIO10_PORT10           = 1,
    GX6605S_GPIO10_TSI1_DATA5       = 2,
    GX6605S_GPIO10_ADCO_CLK         = 3,

    GX6605S_GPIO11_HVSEL            = 0,
    GX6605S_GPIO11_PORT11           = 1,
    GX6605S_GPIO11_TSI1_DATA6       = 2,

    GX6605S_GPIO12_DISEQCO          = 0,
    GX6605S_GPIO12_PORT12           = 1,
    GX6605S_GPIO12_TSI1_DATA7       = 2,

    GX6605S_GPIO13_AGC              = 0,
    GX6605S_GPIO13_PORT13           = 1,
    GX6605S_GPIO13_TSI1_CLK         = 2,

    GX6605S_GPIO14_I2C1_SDA         = 0,
    GX6605S_GPIO14_PORT14           = 1,

    GX6605S_GPIO15_I2C1_SCL         = 0,
    GX6605S_GPIO15_PORT15           = 1,

    GX6605S_GPIO16_UART1_TX         = 0,
    GX6605S_GPIO16_PORT16           = 1,
    GX6605S_GPIO16_AUART_TX         = 2,
    GX6605S_GPIO16_HDMI_DBT0        = 3,

    GX6605S_GPIO17_UART1_RX         = 0,
    GX6605S_GPIO17_PORT17           = 1,
    GX6605S_GPIO17_HDMI_DBT1        = 2,

    GX6605S_GPIO18_DDC_SDA          = 0,
    GX6605S_GPIO18_PORT18           = 1,

    GX6605S_GPIO19_DDC_SCL          = 0,
    GX6605S_GPIO19_PORT19           = 1,

    GX6605S_GPIO20_SPDIF            = 0,
    GX6605S_GPIO20_PORT20           = 1,
    GX6605S_GPIO20_OTP_AVDDEN       = 2,
};

/************************************************************************************************/
/*      Mnemonic                            value                  meaning/usage                */

#define GX6605S_DTO_CONFIG_SEL              BIT(31)
#define GX6605S_DTO_CONFIG_SET              BIT(30)
#define GX6605S_DTO_CONFIG_POWERDOWN        BIT(14)

#define GX6605S_CLOCK_DIV0_AUDIO_RST        BIT(31)
#define GX6605S_CLOCK_DIV0_AUDIO_END        BIT(30)
#define GX6605S_CLOCK_DIV0_AUDIO_VAL        BIT_RANGE(29, 23)
#define GX6605S_CLOCK_DIV0_EVEN_RST         BIT(20)
#define GX6605S_CLOCK_DIV0_EVEN_END         BIT(19)
#define GX6605S_CLOCK_DIV0_EVEN_VAL         BIT_RANGE(17, 12)
#define GX6605S_CLOCK_DIV0_SVPU_RST         BIT(11)
#define GX6605S_CLOCK_DIV0_SVPU_VAL         BIT_RANGE(10, 8)
#define GX6605S_CLOCK_DIV0_VPIEX_RST        BIT(7)
#define GX6605S_CLOCK_DIV0_VPIEX_END        BIT(6)
#define GX6605S_CLOCK_DIV0_VPIEX_VAL        BIT_RANGE(5, 0)

#define GX6605S_SOURCE_SEL0_ADC             BIT(31)
#define GX6605S_SOURCE_SEL0_DAC             BIT(27)
#define GX6605S_SOURCE_SEL0_DRAMC           BIT(25)
#define GX6605S_SOURCE_SEL0_CPU             BIT(24)
#define GX6605S_SOURCE_SEL0_UART            BIT(20)
#define GX6605S_SOURCE_SEL0_APB             BIT(19)
#define GX6605S_SOURCE_SEL0_DEMUXSTC        BIT(18)
#define GX6605S_SOURCE_SEL0_DEMUXSYS        BIT(17)
#define GX6605S_SOURCE_SEL0_GA              BIT(16)
#define GX6605S_SOURCE_SEL0_AUDIOD          BIT(15)
#define GX6605S_SOURCE_SEL0_PP              BIT(14)
#define GX6605S_SOURCE_SEL0_JPEG            BIT(13)
#define GX6605S_SOURCE_SEL0_VIDEO           BIT(12)
#define GX6605S_SOURCE_SEL0_VDAC            BIT(8)
#define GX6605S_SOURCE_SEL0_SVPU            BIT(6)
#define GX6605S_SOURCE_SEL0_EVEN            BIT(5)
#define GX6605S_SOURCE_SEL0_AUDIO           BIT(0)

#define GX6605S_DRAM_CTRL0_DM_OCD           BIT_RANGE(26, 24)
#define GX6605S_DRAM_CTRL0_DQ_PD            BIT_RANGE(23, 20)
#define GX6605S_DRAM_CTRL0_DQ_PU            BIT_RANGE(19, 16)
#define GX6605S_DRAM_CTRL0_FEED_OCD2        BIT_RANGE(5, 3)
#define GX6605S_DRAM_CTRL0_FEED_OCD1        BIT_RANGE(5, 3)
#define GX6605S_DRAM_CTRL0_FEED_ODT         BIT_RANGE(2, 0)

#endif  /* __GX6605S_PLATFORM_H__ */
