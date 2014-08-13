/*
 * exynos4412-irq.c
 *
 * Copyright(c) 2007-2014 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <cp15.h>
#include <exynos4412/reg-gpio.h>
#include <exynos4412/reg-gic.h>
#include <exynos4412/reg-combiner.h>
#include <exynos4412/reg-timer.h>

struct pt_regs_t {
	u32_t	r0,		r1,		r2,		r3, 	r4,		r5;
	u32_t	r6,		r7,		r8, 	r9, 	r10,	fp;
	u32_t	ip, 	sp, 	lr, 	pc,		cpsr, 	orig_r0;
};

static struct irq_handler_t exynos4412_irq_handler[160];

void do_irqs(struct pt_regs_t * regs)
{
	u32_t irq;

	/* Get irq's offset */
	irq = readl(EXYNOS4412_GIC_CPU_BASE + GIC_CPU_INTACK) & 0x3ff;

	/* Handle interrupt server function */
	(exynos4412_irq_handler[irq - 32].func)(exynos4412_irq_handler[irq - 32].data);

	/* Exit interrupt */
	writel(EXYNOS4412_GIC_CPU_BASE + GIC_CPU_EOI, irq);
}

static void enable_irqs(struct irq_t * irq, bool_t enable)
{
	u32_t mask = 1 << (irq->irq_no % 32);

	if(enable)
		writel(EXYNOS4412_GIC_DIST_BASE + GIC_DIST_ENABLE_SET + (irq->irq_no / 32) * 4, mask);
	else
		writel(EXYNOS4412_GIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR + (irq->irq_no / 32) * 4, mask);
}

static struct irq_t exynos4412_irqs[] = {
	{
		.name		= "INTG0",
		.irq_no		= 32,
		.handler	= &exynos4412_irq_handler[32 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG1",
		.irq_no		= 33,
		.handler	= &exynos4412_irq_handler[33 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG2",
		.irq_no		= 34,
		.handler	= &exynos4412_irq_handler[34 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG3",
		.irq_no		= 35,
		.handler	= &exynos4412_irq_handler[35 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG4",
		.irq_no		= 36,
		.handler	= &exynos4412_irq_handler[36 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG5",
		.irq_no		= 37,
		.handler	= &exynos4412_irq_handler[37 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG6",
		.irq_no		= 38,
		.handler	= &exynos4412_irq_handler[38 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG7",
		.irq_no		= 39,
		.handler	= &exynos4412_irq_handler[39 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG8",
		.irq_no		= 40,
		.handler	= &exynos4412_irq_handler[40 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG9",
		.irq_no		= 41,
		.handler	= &exynos4412_irq_handler[41 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG10",
		.irq_no		= 42,
		.handler	= &exynos4412_irq_handler[42 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG11",
		.irq_no		= 43,
		.handler	= &exynos4412_irq_handler[43 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG12",
		.irq_no		= 44,
		.handler	= &exynos4412_irq_handler[44 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG13",
		.irq_no		= 45,
		.handler	= &exynos4412_irq_handler[45 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG14",
		.irq_no		= 46,
		.handler	= &exynos4412_irq_handler[46 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG15",
		.irq_no		= 47,
		.handler	= &exynos4412_irq_handler[47 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT0",
		.irq_no		= 48,
		.handler	= &exynos4412_irq_handler[48 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT1",
		.irq_no		= 49,
		.handler	= &exynos4412_irq_handler[49 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT2",
		.irq_no		= 50,
		.handler	= &exynos4412_irq_handler[50 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT3",
		.irq_no		= 51,
		.handler	= &exynos4412_irq_handler[51 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT4",
		.irq_no		= 52,
		.handler	= &exynos4412_irq_handler[52 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT5",
		.irq_no		= 53,
		.handler	= &exynos4412_irq_handler[53 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT6",
		.irq_no		= 54,
		.handler	= &exynos4412_irq_handler[54 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT7",
		.irq_no		= 55,
		.handler	= &exynos4412_irq_handler[55 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT8",
		.irq_no		= 56,
		.handler	= &exynos4412_irq_handler[56 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT9",
		.irq_no		= 57,
		.handler	= &exynos4412_irq_handler[57 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT10",
		.irq_no		= 58,
		.handler	= &exynos4412_irq_handler[58 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT11",
		.irq_no		= 59,
		.handler	= &exynos4412_irq_handler[59 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT12",
		.irq_no		= 60,
		.handler	= &exynos4412_irq_handler[60 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT13",
		.irq_no		= 61,
		.handler	= &exynos4412_irq_handler[61 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT14",
		.irq_no		= 62,
		.handler	= &exynos4412_irq_handler[62 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT15",
		.irq_no		= 63,
		.handler	= &exynos4412_irq_handler[63 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "EINT16_31",
		.irq_no		= 64,
		.handler	= &exynos4412_irq_handler[64 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "C2C_SSCM0",
		.irq_no		= 65,
		.handler	= &exynos4412_irq_handler[65 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "MDMA",
		.irq_no		= 66,
		.handler	= &exynos4412_irq_handler[66 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "PDMA0",
		.irq_no		= 67,
		.handler	= &exynos4412_irq_handler[67 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "PDMA1",
		.irq_no		= 68,
		.handler	= &exynos4412_irq_handler[68 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER0",
		.irq_no		= 69,
		.handler	= &exynos4412_irq_handler[69 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER1",
		.irq_no		= 70,
		.handler	= &exynos4412_irq_handler[70 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER2",
		.irq_no		= 71,
		.handler	= &exynos4412_irq_handler[71 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER3",
		.irq_no		= 72,
		.handler	= &exynos4412_irq_handler[72 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "TIMER4",
		.irq_no		= 73,
		.handler	= &exynos4412_irq_handler[73 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG19",
		.irq_no		= 74,
		.handler	= &exynos4412_irq_handler[74 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "WDT",
		.irq_no		= 75,
		.handler	= &exynos4412_irq_handler[75 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "RTC_ALARM",
		.irq_no		= 76,
		.handler	= &exynos4412_irq_handler[76 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "RTC_TIC",
		.irq_no		= 77,
		.handler	= &exynos4412_irq_handler[77 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "GPIO_RT",
		.irq_no		= 78,
		.handler	= &exynos4412_irq_handler[78 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "GPIO_LB",
		.irq_no		= 79,
		.handler	= &exynos4412_irq_handler[79 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG18",
		.irq_no		= 80,
		.handler	= &exynos4412_irq_handler[80 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "IEM_APC",
		.irq_no		= 81,
		.handler	= &exynos4412_irq_handler[81 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "IEM_IEC",
		.irq_no		= 82,
		.handler	= &exynos4412_irq_handler[82 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "NFC",
		.irq_no		= 83,
		.handler	= &exynos4412_irq_handler[83 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "UART0",
		.irq_no		= 84,
		.handler	= &exynos4412_irq_handler[84 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "UART1",
		.irq_no		= 85,
		.handler	= &exynos4412_irq_handler[85 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "UART2",
		.irq_no		= 86,
		.handler	= &exynos4412_irq_handler[86 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "UART3",
		.irq_no		= 87,
		.handler	= &exynos4412_irq_handler[87 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "G0_IRQ",
		.irq_no		= 89,
		.handler	= &exynos4412_irq_handler[89 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C0",
		.irq_no		= 90,
		.handler	= &exynos4412_irq_handler[90 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C1",
		.irq_no		= 91,
		.handler	= &exynos4412_irq_handler[91 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C2",
		.irq_no		= 92,
		.handler	= &exynos4412_irq_handler[92 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C3",
		.irq_no		= 93,
		.handler	= &exynos4412_irq_handler[93 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C4",
		.irq_no		= 94,
		.handler	= &exynos4412_irq_handler[94 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C5",
		.irq_no		= 95,
		.handler	= &exynos4412_irq_handler[95 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C6",
		.irq_no		= 96,
		.handler	= &exynos4412_irq_handler[96 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "I2C7",
		.irq_no		= 97,
		.handler	= &exynos4412_irq_handler[97 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "SPI0",
		.irq_no		= 98,
		.handler	= &exynos4412_irq_handler[98 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "SPI1",
		.irq_no		= 99,
		.handler	= &exynos4412_irq_handler[99 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "SPI2",
		.irq_no		= 100,
		.handler	= &exynos4412_irq_handler[100 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "G1_IRQ",
		.irq_no		= 101,
		.handler	= &exynos4412_irq_handler[101 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "UHOST",
		.irq_no		= 102,
		.handler	= &exynos4412_irq_handler[102 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "HSOTG",
		.irq_no		= 103,
		.handler	= &exynos4412_irq_handler[103 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "GPIO_C2C",
		.irq_no		= 104,
		.handler	= &exynos4412_irq_handler[104 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "HSMMC0",
		.irq_no		= 105,
		.handler	= &exynos4412_irq_handler[105 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "HSMMC1",
		.irq_no		= 106,
		.handler	= &exynos4412_irq_handler[106 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "HSMMC2",
		.irq_no		= 107,
		.handler	= &exynos4412_irq_handler[107 - 32],
		.enable		= enable_irqs,
	},{
		.name		= "HSMMC3",
		.irq_no		= 108,
		.handler	= &exynos4412_irq_handler[108 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "SDMMC",
		.irq_no		= 109,
		.handler	= &exynos4412_irq_handler[109 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "MIPI_CSI_4LANE",
		.irq_no		= 110,
		.handler	= &exynos4412_irq_handler[110 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "MIPI_DSI_4LANE",
		.irq_no		= 111,
		.handler	= &exynos4412_irq_handler[111 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "MIPI_CSI_2LANE",
		.irq_no		= 112,
		.handler	= &exynos4412_irq_handler[112 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "ROTATOR",
		.irq_no		= 115,
		.handler	= &exynos4412_irq_handler[115 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "FIMC0",
		.irq_no		= 116,
		.handler	= &exynos4412_irq_handler[116 - 32],
		.enable		= enable_irqs,
	},{
		.name		= "FIMC1",
		.irq_no		= 117,
		.handler	= &exynos4412_irq_handler[117 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "FIMC2",
		.irq_no		= 118,
		.handler	= &exynos4412_irq_handler[118 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "FIMC3",
		.irq_no		= 119,
		.handler	= &exynos4412_irq_handler[119 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "JPEG",
		.irq_no		= 120,
		.handler	= &exynos4412_irq_handler[120 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "G2D",
		.irq_no		= 121,
		.handler	= &exynos4412_irq_handler[121 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "ISP0",
		.irq_no		= 122,
		.handler	= &exynos4412_irq_handler[122 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "MIXER",
		.irq_no		= 123,
		.handler	= &exynos4412_irq_handler[123 - 32],
		.enable		= enable_irqs,
	},{
		.name		= "HDMI",
		.irq_no		= 124,
		.handler	= &exynos4412_irq_handler[124 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "HDMI_I2C",
		.irq_no		= 125,
		.handler	= &exynos4412_irq_handler[125 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "MFC",
		.irq_no		= 126,
		.handler	= &exynos4412_irq_handler[126 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "ISP1",
		.irq_no		= 127,
		.handler	= &exynos4412_irq_handler[127 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "AUDIO_SS",
		.irq_no		= 128,
		.handler	= &exynos4412_irq_handler[128 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "I2S0",
		.irq_no		= 129,
		.handler	= &exynos4412_irq_handler[129 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "I2S1",
		.irq_no		= 130,
		.handler	= &exynos4412_irq_handler[130 - 32],
		.enable		= enable_irqs,
	},{
		.name		= "I2S2",
		.irq_no		= 131,
		.handler	= &exynos4412_irq_handler[131 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "AC97",
		.irq_no		= 132,
		.handler	= &exynos4412_irq_handler[132 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "PCM0",
		.irq_no		= 133,
		.handler	= &exynos4412_irq_handler[133 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "PCM1",
		.irq_no		= 134,
		.handler	= &exynos4412_irq_handler[134 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "PCM2",
		.irq_no		= 135,
		.handler	= &exynos4412_irq_handler[135 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "SPDIF",
		.irq_no		= 136,
		.handler	= &exynos4412_irq_handler[136 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "FIMC_LITE0",
		.irq_no		= 137,
		.handler	= &exynos4412_irq_handler[137 - 32],
		.enable		= enable_irqs,
	},{
		.name		= "FIMC_LITE1",
		.irq_no		= 138,
		.handler	= &exynos4412_irq_handler[138 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG16",
		.irq_no		= 139,
		.handler	= &exynos4412_irq_handler[139 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "INTG17",
		.irq_no		= 140,
		.handler	= &exynos4412_irq_handler[140 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "KEYPAD",
		.irq_no		= 141,
		.handler	= &exynos4412_irq_handler[141 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "PMU",
		.irq_no		= 142,
		.handler	= &exynos4412_irq_handler[142 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "GPS",
		.irq_no		= 143,
		.handler	= &exynos4412_irq_handler[143 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "SSS",
		.irq_no		= 144,
		.handler	= &exynos4412_irq_handler[144 - 32],
		.enable		= enable_irqs,
	},{
		.name		= "SLIMBUS",
		.irq_no		= 145,
		.handler	= &exynos4412_irq_handler[145 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "CEC",
		.irq_no		= 146,
		.handler	= &exynos4412_irq_handler[146 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "TSI",
		.irq_no		= 147,
		.handler	= &exynos4412_irq_handler[147 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "C2C_SSCM1",
		.irq_no		= 148,
		.handler	= &exynos4412_irq_handler[148 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "G3D_IRQPMU",
		.irq_no		= 149,
		.handler	= &exynos4412_irq_handler[149 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "G3D_IRQPPMMU0",
		.irq_no		= 150,
		.handler	= &exynos4412_irq_handler[150 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "G3D_IRQPPMMU1",
		.irq_no		= 151,
		.handler	= &exynos4412_irq_handler[151 - 32],
		.enable		= enable_irqs,
	},{
		.name		= "G3D_IRQPPMMU2",
		.irq_no		= 152,
		.handler	= &exynos4412_irq_handler[152 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "G3D_IRQPPMMU3",
		.irq_no		= 153,
		.handler	= &exynos4412_irq_handler[153 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "G3D_IRQGPMMU",
		.irq_no		= 154,
		.handler	= &exynos4412_irq_handler[154 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "G3D_IRQPP0",
		.irq_no		= 155,
		.handler	= &exynos4412_irq_handler[155 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "G3D_IRQPP1",
		.irq_no		= 156,
		.handler	= &exynos4412_irq_handler[156 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "G3D_IRQPP2",
		.irq_no		= 157,
		.handler	= &exynos4412_irq_handler[157 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "G3D_IRQPP3",
		.irq_no		= 158,
		.handler	= &exynos4412_irq_handler[158 - 32],
		.enable		= enable_irqs,
	}, {
		.name		= "G3D_IRQGP",
		.irq_no		= 159,
		.handler	= &exynos4412_irq_handler[159 - 32],
		.enable		= enable_irqs,
	},
};

static void gic_dist_init(physical_addr_t dist)
{
	u32_t gic_irqs;
	u32_t cpumask;
	u32_t i;

	writel(dist + GIC_DIST_CTRL, 0x0);

	/*
	 * Find out how many interrupts are supported.
	 * The GIC only supports up to 1020 interrupt sources.
	 */
	gic_irqs = readl(dist + GIC_DIST_CTR) & 0x1f;
	gic_irqs = (gic_irqs + 1) * 32;
	if(gic_irqs > 1020)
		gic_irqs = 1020;

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for(i = 32; i < gic_irqs; i += 16)
		writel(dist + GIC_DIST_CONFIG + i * 4 / 16, 0);

	/*
	 * Set all global interrupts to this CPU only.
	 */
	cpumask = 1 << get_cpuid();
	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;

	for(i = 32; i < gic_irqs; i += 4)
		writel(dist + GIC_DIST_TARGET + i * 4 / 4, cpumask);

	/*
	 * Set priority on all global interrupts.
	 */
	for(i = 32; i < gic_irqs; i += 4)
		writel(dist + GIC_DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	/*
	 * Disable all interrupts.  Leave the PPI and SGIs alone
	 * as these enables are banked registers.
	 */
	for(i = 32; i < gic_irqs; i += 32)
		writel(dist + GIC_DIST_ENABLE_CLEAR + i * 4 / 32, 0xffffffff);

	writel(dist + GIC_DIST_CTRL, 0x1);
}

static void gic_cpu_init(physical_addr_t dist, physical_addr_t cpu)
{
	int i;

	/*
	 * Deal with the banked PPI and SGI interrupts - disable all
	 * PPI interrupts, ensure all SGI interrupts are enabled.
	 */
	writel(dist + GIC_DIST_ENABLE_CLEAR, 0xffff0000);
	writel(dist + GIC_DIST_ENABLE_SET, 0x0000ffff);

	/*
	 * Set priority on PPI and SGI interrupts
	 */
	for(i = 0; i < 32; i += 4)
		writel(dist + GIC_DIST_PRI + i * 4 / 4, 0xa0a0a0a0);

	writel(cpu + GIC_CPU_PRIMASK, 0xf0);
	writel(cpu + GIC_CPU_CTRL, 0x1);
}

static void combiner_init(physical_addr_t comb)
{
	int i;

	for(i = 0; i < 5; i++)
		writel(comb + COMBINER_ENABLE_CLEAR + i * 0x10, 0xffffffff);
}

static __init void exynos4412_irq_init(void)
{
	int i;

	gic_dist_init(EXYNOS4412_GIC_DIST_BASE);
	gic_cpu_init(EXYNOS4412_GIC_DIST_BASE, EXYNOS4412_GIC_CPU_BASE);
	combiner_init(EXYNOS4412_COMBINER_BASE);

	for(i = 0; i < ARRAY_SIZE(exynos4412_irqs); i++)
	{
		if(irq_register(&exynos4412_irqs[i]))
			LOG("Register irq '%s'", exynos4412_irqs[i].name);
		else
			LOG("Failed to register irq '%s'", exynos4412_irqs[i].name);
	}

	/* Enable vector interrupt controller */
	vic_enable();

	/* Enable irq and fiq */
	irq_enable();
	fiq_enable();
}

static __exit void exynos4412_irq_exit(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(exynos4412_irqs); i++)
	{
		if(irq_unregister(&exynos4412_irqs[i]))
			LOG("Unregister irq '%s'", exynos4412_irqs[i].name);
		else
			LOG("Failed to unregister irq '%s'", exynos4412_irqs[i].name);
	}

	/* Disable vector interrupt controller */
	vic_disable();

	/* Disable irq and fiq */
	irq_disable();
	fiq_disable();
}

core_initcall(exynos4412_irq_init);
core_exitcall(exynos4412_irq_exit);
