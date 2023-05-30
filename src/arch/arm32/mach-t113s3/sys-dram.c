#include <xboot.h>
#include <t113/reg-ccu.h>
#include <t113/reg-dram.h>

#define clrbits_le32(addr, clear) \
	write32(((virtual_addr_t)(addr)), read32(((virtual_addr_t)(addr))) & ~(clear))

#define setbits_le32(addr, set) \
	write32(((virtual_addr_t)(addr)), read32(((virtual_addr_t)(addr))) | (set))

#define clrsetbits_le32(addr, clear, set) \
	write32(((virtual_addr_t)(addr)), (read32(((virtual_addr_t)(addr))) & ~(clear)) | (set))

#define CONFIG_DRAM_BASE	(0x40000000)
#define DIV_ROUND_UP(n, d)	(((n) + (d) - 1) / (d))

struct dram_param_t {
	uint32_t dram_clk;
	uint32_t dram_type;
	uint32_t dram_zq;
	uint32_t dram_odt_en;
	uint32_t dram_para1;
	uint32_t dram_para2;
	uint32_t dram_mr0;
	uint32_t dram_mr1;
	uint32_t dram_mr2;
	uint32_t dram_mr3;
	uint32_t dram_tpr0;
	uint32_t dram_tpr1;
	uint32_t dram_tpr2;
	uint32_t dram_tpr3;
	uint32_t dram_tpr4;
	uint32_t dram_tpr5;
	uint32_t dram_tpr6;
	uint32_t dram_tpr7;
	uint32_t dram_tpr8;
	uint32_t dram_tpr9;
	uint32_t dram_tpr10;
	uint32_t dram_tpr11;
	uint32_t dram_tpr12;
	uint32_t dram_tpr13;
	uint32_t reserve[8];
};

enum dram_type_t {
	DRAM_TYPE_DDR2   = 2,
	DRAM_TYPE_DDR3   = 3,
	DRAM_TYPE_LPDDR2 = 6,
	DRAM_TYPE_LPDDR3 = 7,
};

static inline void sdelay(int loops)
{
	__asm__ __volatile__ ("1:\n" "subs %0, %1, #1\n"
		"bne 1b":"=r" (loops):"0"(loops));
}

static inline int ns_to_t(struct dram_param_t * para, int ns)
{
	unsigned int freq = para->dram_clk >> 1;
	return DIV_ROUND_UP(freq * ns, 1000);
}

static inline void sid_read_ldob_cal(struct dram_param_t * para)
{
	uint32_t reg;

	reg = (read32(SYS_SID_BASE + SYS_LDOB_SID) & 0xff00) >> 8;
	if(reg == 0)
		return;
	if(para->dram_type == DRAM_TYPE_DDR2)
	{
	}
	else if(para->dram_type == DRAM_TYPE_DDR3)
	{
		if(reg > 0x20)
			reg -= 0x16;
	}
	else
	{
		reg = 0;
	}
	clrsetbits_le32((SYS_CONTROL_REG_BASE + LDO_CTAL_REG), 0xff00, reg << 8);
}

static inline void dram_voltage_set(struct dram_param_t * para)
{
	int vol;

	if(para->dram_type == DRAM_TYPE_DDR2)
		vol = 47;
	else if(para->dram_type == DRAM_TYPE_DDR3)
		vol = 25;
	else
		vol = 0;
	clrsetbits_le32((SYS_CONTROL_REG_BASE + LDO_CTAL_REG), 0x20ff00, vol << 8);
	sdelay(1);
	sid_read_ldob_cal(para);
}

static inline void dram_enable_all_master(void)
{
	write32((MCTL_COM_BASE + MCTL_COM_MAER0), 0xffffffff);
	write32((MCTL_COM_BASE + MCTL_COM_MAER1), 0x000000ff);
	write32((MCTL_COM_BASE + MCTL_COM_MAER2), 0x0000ffff);
	sdelay(10);
}

static inline void dram_disable_all_master(void)
{
	write32((MCTL_COM_BASE + MCTL_COM_MAER0), 0x1);
	write32((MCTL_COM_BASE + MCTL_COM_MAER1), 0x0);
	write32((MCTL_COM_BASE + MCTL_COM_MAER2), 0x0);
	sdelay(10);
}

static void eye_delay_compensation(struct dram_param_t * para)
{
	uint32_t delay;
	int i;

	delay = (para->dram_tpr11 & 0xf) << 9;
	delay |= (para->dram_tpr12 & 0xf) << 1;
	for(i = 0; i < 9; i++)
		setbits_le32((MCTL_PHY_BASE + MCTL_PHY_DATX0IOCR(i)), delay);
	delay = (para->dram_tpr11 & 0xf0) << 5;
	delay |= (para->dram_tpr12 & 0xf0) >> 3;
	for(i = 0; i < 9; i++)
		setbits_le32((MCTL_PHY_BASE + MCTL_PHY_DATX1IOCR(i)), delay);
	clrbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR0), 0x04000000);
	delay = (para->dram_tpr11 & 0xf0000) >> 7;
	delay |= (para->dram_tpr12 & 0xf0000) >> 15;
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_DATX0IOCR(9)), delay);
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_DATX0IOCR(10)), delay);
	delay = (para->dram_tpr11 & 0xf00000) >> 11;
	delay |= (para->dram_tpr12 & 0xf00000) >> 19;
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_DATX1IOCR(9)), delay);
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_DATX1IOCR(10)), delay);
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_DXnSDLR6(0)), (para->dram_tpr11 & 0xf0000) << 9);
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_DXnSDLR6(1)), (para->dram_tpr11 & 0xf00000) << 5);
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR0), (1 << 26));
	sdelay(1);
	delay = (para->dram_tpr10 & 0xf0) << 4;
	for(i = 6; i < 27; ++i)
		setbits_le32((MCTL_PHY_BASE + MCTL_PHY_ACIOCR1(i)), delay);
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_ACIOCR1(2)), (para->dram_tpr10 & 0x0f) << 8);
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_ACIOCR1(3)), (para->dram_tpr10 & 0x0f) << 8);
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_ACIOCR1(28)), (para->dram_tpr10 & 0xf00) >> 4);
}

static void mctl_set_timing_params(struct dram_param_t * para)
{
	uint8_t tccd = 2;
	uint8_t tfaw;
	uint8_t trrd;
	uint8_t trcd;
	uint8_t trc;
	uint8_t txp;
	uint8_t twtr;
	uint8_t trtp = 4;
	uint8_t twr;
	uint8_t trp;
	uint8_t tras;
	uint16_t trefi;
	uint16_t trfc;
	uint8_t tcksrx;
	uint8_t tckesr;
	uint8_t trd2wr;
	uint8_t twr2rd;
	uint8_t trasmax;
	uint8_t twtp;
	uint8_t tcke;
	uint8_t tmod;
	uint8_t tmrd;
	uint8_t tmrw;
	uint8_t tcl;
	uint8_t tcwl;
	uint8_t t_rdata_en;
	uint8_t wr_latency;
	uint32_t mr0;
	uint32_t mr1;
	uint32_t mr2;
	uint32_t mr3;
	uint32_t tdinit0;
	uint32_t tdinit1;
	uint32_t tdinit2;
	uint32_t tdinit3;

	if(para->dram_type == DRAM_TYPE_DDR2)
	{
		tfaw = ns_to_t(para, 50);
		trrd = ns_to_t(para, 10);
		trcd = ns_to_t(para, 20);
		trc = ns_to_t(para, 65);
		txp = 2;
		twtr = ns_to_t(para, 8);
		twr = ns_to_t(para, 15);
		trp = ns_to_t(para, 15);
		tras = ns_to_t(para, 45);
		trfc = ns_to_t(para, 328);
		trefi = ns_to_t(para, 7800) / 32;
		trasmax = para->dram_clk / 30;
		if(para->dram_clk < 409)
		{
			t_rdata_en = 1;
			tcl = 3;
			mr0 = 0x06a3;
		}
		else
		{
			t_rdata_en = 2;
			tcl = 4;
			mr0 = 0x0e73;
		}
		tmrd = 2;
		twtp = twr + 5;
		tcksrx = 5;
		tckesr = 4;
		trd2wr = 4;
		tcke = 3;
		tmod = 12;
		wr_latency = 1;
		tmrw = 0;
		twr2rd = twtr + 5;
		tcwl = 0;
		mr1 = para->dram_mr1;
		mr2 = 0;
		mr3 = 0;
		tdinit0 = 200 * para->dram_clk + 1;
		tdinit1 = 100 * para->dram_clk / 1000 + 1;
		tdinit2 = 200 * para->dram_clk + 1;
		tdinit3 = 1 * para->dram_clk + 1;
	}
	else if(para->dram_type == DRAM_TYPE_DDR3)
	{
		trfc = ns_to_t(para, 350);
		trefi = ns_to_t(para, 7800) / 32 + 1;
		twtr = ns_to_t(para, 8) + 2;
		trrd = max(ns_to_t(para, 10), 2);
		txp = max(ns_to_t(para, 10), 2);
		if (para->dram_clk <= 800)
		{
			tfaw = ns_to_t(para, 50);
			trcd = ns_to_t(para, 15);
			trp = ns_to_t(para, 15);
			trc = ns_to_t(para, 53);
			tras = ns_to_t(para, 38);
			mr0 = 0x1c70;
			mr2 = 0x18;
			tcl = 6;
			wr_latency = 2;
			tcwl = 4;
			t_rdata_en = 4;
		}
		else
		{
			tfaw = ns_to_t(para, 35);
			trcd = ns_to_t(para, 14);
			trp = ns_to_t(para, 14);
			trc = ns_to_t(para, 48);
			tras = ns_to_t(para, 34);
			mr0 = 0x1e14;
			mr2 = 0x20;
			tcl = 7;
			wr_latency = 3;
			tcwl = 5;
			t_rdata_en = 5;
		}
		trasmax = para->dram_clk / 30;
		twtp = tcwl + 2 + twtr;
		twr2rd = tcwl + twtr;
		tdinit0 = 500 * para->dram_clk + 1;
		tdinit1 = 360 * para->dram_clk / 1000 + 1;
		tdinit2 = 200 * para->dram_clk + 1;
		tdinit3 = 1 * para->dram_clk + 1;
		mr1 = para->dram_mr1;
		mr3 = 0;
		tcke = 3;
		tcksrx = 5;
		tckesr = 4;
		if(((para->dram_tpr13 & 0xc) == 0x04) || para->dram_clk < 912)
			trd2wr = 5;
		else
			trd2wr = 6;
		tmod = 12;
		tmrd = 4;
		tmrw = 0;
	}
	else if(para->dram_type == DRAM_TYPE_LPDDR2)
	{
		tfaw = max(ns_to_t(para, 50), 4);
		trrd = max(ns_to_t(para, 10), 1);
		trcd = max(ns_to_t(para, 24), 2);
		trc = ns_to_t(para, 70);
		txp = ns_to_t(para, 8);
		if(txp < 2)
		{
			txp++;
			twtr = 2;
		}
		else
		{
			twtr = txp;
		}
		twr = max(ns_to_t(para, 15), 2);
		trp = ns_to_t(para, 17);
		tras = ns_to_t(para, 42);
		trefi = ns_to_t(para, 3900) / 32;
		trfc = ns_to_t(para, 210);
		trasmax = para->dram_clk / 60;
		mr3 = para->dram_mr3;
		twtp = twr + 5;
		mr2 = 6;
		mr1 = 5;
		tcksrx = 5;
		tckesr = 5;
		trd2wr = 10;
		tcke = 2;
		tmod = 5;
		tmrd = 5;
		tmrw = 3;
		tcl = 4;
		wr_latency = 1;
		t_rdata_en = 1;
		tdinit0 = 200 * para->dram_clk + 1;
		tdinit1 = 100 * para->dram_clk / 1000 + 1;
		tdinit2 = 11 * para->dram_clk + 1;
		tdinit3 = 1 * para->dram_clk + 1;
		twr2rd = twtr + 5;
		tcwl = 2;
		mr1 = 195;
		mr0 = 0;
	}
	else if(para->dram_type == DRAM_TYPE_LPDDR3)
	{
		tfaw = max(ns_to_t(para, 50), 4);
		trrd = max(ns_to_t(para, 10), 1);
		trcd = max(ns_to_t(para, 24), 2);
		trc = ns_to_t(para, 70);
		twtr = max(ns_to_t(para, 8), 2);
		twr = max(ns_to_t(para, 15), 2);
		trp = ns_to_t(para, 17);
		tras = ns_to_t(para, 42);
		trefi = ns_to_t(para, 3900) / 32;
		trfc = ns_to_t(para, 210);
		txp = twtr;
		trasmax = para->dram_clk / 60;
		if (para->dram_clk < 800)
		{
			tcwl = 4;
			wr_latency = 3;
			t_rdata_en = 6;
			mr2 = 12;
		}
		else
		{
			tcwl = 3;
			tcke = 6;
			wr_latency = 2;
			t_rdata_en = 5;
			mr2 = 10;
		}
		twtp = tcwl + 5;
		tcl = 7;
		mr3 = para->dram_mr3;
		tcksrx = 5;
		tckesr = 5;
		trd2wr = 13;
		tcke = 3;
		tmod = 12;
		tdinit0 = 400 * para->dram_clk + 1;
		tdinit1 = 500 * para->dram_clk / 1000 + 1;
		tdinit2 = 11 * para->dram_clk + 1;
		tdinit3 = 1 * para->dram_clk + 1;
		tmrd = 5;
		tmrw = 5;
		twr2rd = tcwl + twtr + 5;
		mr1 = 195;
		mr0 = 0;
	}
	else
	{
		trfc = 128;
		trp = 6;
		trefi = 98;
		txp = 10;
		twr = 8;
		twtr = 3;
		tras = 14;
		tfaw = 16;
		trc = 20;
		trcd = 6;
		trrd = 3;
		twr2rd = 8;
		tcksrx = 4;
		tckesr = 3;
		trd2wr = 4;
		trasmax = 27;
		twtp = 12;
		tcke = 2;
		tmod = 6;
		tmrd = 2;
		tmrw = 0;
		tcwl = 3;
		tcl = 3;
		wr_latency = 1;
		t_rdata_en = 1;
		mr3 = 0;
		mr2 = 0;
		mr1 = 0;
		mr0 = 0;
		tdinit3 = 0;
		tdinit2 = 0;
		tdinit1 = 0;
		tdinit0 = 0;
	}

	/* Set mode registers */
	write32((MCTL_PHY_BASE + MCTL_PHY_DRAM_MR0), mr0);
	write32((MCTL_PHY_BASE + MCTL_PHY_DRAM_MR1), mr1);
	write32((MCTL_PHY_BASE + MCTL_PHY_DRAM_MR2), mr2);
	write32((MCTL_PHY_BASE + MCTL_PHY_DRAM_MR3), mr3);
	write32((MCTL_PHY_BASE + MCTL_PHY_LP3MR11), (para->dram_odt_en >> 4) & 0x3);

	/* Set dram timing DRAMTMG0 - DRAMTMG5 */
	write32((MCTL_PHY_BASE + MCTL_PHY_DRAMTMG0), (twtp << 24) | (tfaw << 16) | (trasmax << 8) | (tras << 0));
	write32((MCTL_PHY_BASE + MCTL_PHY_DRAMTMG1), (txp << 16) | (trtp << 8) | (trc << 0));
	write32((MCTL_PHY_BASE + MCTL_PHY_DRAMTMG2), (tcwl << 24) | (tcl << 16) | (trd2wr << 8) | (twr2rd << 0));
	write32((MCTL_PHY_BASE + MCTL_PHY_DRAMTMG3), (tmrw << 16) | (tmrd << 12) | (tmod << 0));
	write32((MCTL_PHY_BASE + MCTL_PHY_DRAMTMG4), (trcd << 24) | (tccd << 16) | (trrd << 8) | (trp << 0));
	write32((MCTL_PHY_BASE + MCTL_PHY_DRAMTMG5), (tcksrx << 24) | (tcksrx << 16) | (tckesr << 8) | (tcke << 0));

	/* Set dual rank timing */
	clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_DRAMTMG8), 0xf000ffff, (para->dram_clk < 800) ? 0xf0006610 : 0xf0007610);

	/* Set phy interface time PITMG0, PTR3, PTR4 */
	write32((MCTL_PHY_BASE + MCTL_PHY_PITMG0), (0x2 << 24) | (t_rdata_en << 16) | (1 << 8) | (wr_latency << 0));
	write32((MCTL_PHY_BASE + MCTL_PHY_PTR3), ((tdinit0 << 0) | (tdinit1 << 20)));
	write32((MCTL_PHY_BASE + MCTL_PHY_PTR4), ((tdinit2 << 0) | (tdinit3 << 20)));

	/* Set refresh timing and mode */
	write32((MCTL_PHY_BASE + MCTL_PHY_RFSHTMG), (trefi << 16) | (trfc << 0));
	write32((MCTL_PHY_BASE + MCTL_PHY_RFSHCTL1), (trefi << 15) & 0x0fff0000);
}

static int ccu_set_pll_ddr_clk(int index, struct dram_param_t * para)
{
	unsigned int val, clk, n;

	if(para->dram_tpr13 & (1 << 6))
		clk = para->dram_tpr9;
	else
		clk = para->dram_clk;
	n = (clk * 2) / 24;
	val = read32((T113_CCU_BASE + CCU_PLL_DDR_CTRL_REG));
	val &= 0xfff800fc;
	val |= (n - 1) << 8;
	val |= 0xc0000000;
	val &= 0xdfffffff;
	write32((T113_CCU_BASE + CCU_PLL_DDR_CTRL_REG), val | 0x20000000);
	while((read32((T113_CCU_BASE + CCU_PLL_DDR_CTRL_REG)) & 0x10000000) == 0);
	sdelay(20);
	val = read32(T113_CCU_BASE);
	val |= 0x08000000;
	write32(T113_CCU_BASE, val);
	val = read32((T113_CCU_BASE + CCU_DRAM_CLK_REG));
	val &= 0xfcfffcfc;
	val |= 0x80000000;
	write32((T113_CCU_BASE + CCU_DRAM_CLK_REG), val);
	return n * 24;
}

static void mctl_sys_init(struct dram_param_t * para)
{
	clrbits_le32((T113_CCU_BASE + CCU_MBUS_CLK_REG), (1 << 30));
	clrbits_le32((T113_CCU_BASE + CCU_DRAM_BGR_REG), 0x10001);
	clrsetbits_le32((T113_CCU_BASE + CCU_DRAM_CLK_REG), (1 << 31) | (1 << 30), (1 << 27));
	sdelay(10);
	para->dram_clk = ccu_set_pll_ddr_clk(0, para) / 2;
	sdelay(100);
	dram_disable_all_master();
	setbits_le32((T113_CCU_BASE + CCU_DRAM_BGR_REG), (1 << 16));
	setbits_le32((T113_CCU_BASE + CCU_MBUS_CLK_REG), (1 << 30));
	setbits_le32((T113_CCU_BASE + CCU_DRAM_CLK_REG), (1 << 30));
	sdelay(5);
	setbits_le32((T113_CCU_BASE + CCU_DRAM_BGR_REG), (1 << 0));
	setbits_le32((T113_CCU_BASE + CCU_DRAM_CLK_REG), (1 << 31) | (1 << 27));
	sdelay(5);
	write32((MCTL_PHY_BASE + MCTL_PHY_CLKEN), 0x8000);
	sdelay(10);
}

static void mctl_com_init(struct dram_param_t * para)
{
	virtual_addr_t ptr;
	uint32_t val, width;
	uint32_t t;
	int i;

	clrsetbits_le32((MCTL_COM_BASE + MCTL_COM_DBGCR), 0x3f00, 0x2000);
	val = read32((MCTL_COM_BASE + MCTL_COM_WORK_MODE0)) & ~0x00fff000;
	val |= (para->dram_type & 0x7) << 16;
	val |= (~para->dram_para2 & 0x1) << 12;
	val |= (1 << 22);
	if(para->dram_type == DRAM_TYPE_LPDDR2 || para->dram_type == DRAM_TYPE_LPDDR3)
	{
		val |= (1 << 19);
	}
	else
	{
		if(para->dram_tpr13 & (1 << 5))
			val |= (1 << 19);
	}
	write32((MCTL_COM_BASE + MCTL_COM_WORK_MODE0), val);
	if((para->dram_para2 & (1 << 8)) && ((para->dram_para2 & 0xf000) != 0x1000))
		width = 32;
	else
		width = 16;
	ptr = (MCTL_COM_BASE + MCTL_COM_WORK_MODE0);
	for(i = 0; i < width; i += 16)
	{
		val = read32(ptr) & 0xfffff000;
		val |= (para->dram_para2 >> 12) & 0x3;
		val |= ((para->dram_para1 >> (i + 12)) << 2) & 0x4;
		val |= (((para->dram_para1 >> (i + 4)) - 1) << 4) & 0xff;
		t = (para->dram_para1 >> i) & 0xf;
		if(t == 8)
			val |= 0xa00;
		else if(t == 4)
			val |= 0x900;
		else if(t == 2)
			val |= 0x800;
		else if(t == 1)
			val |= 0x700;
		else
			val |= 0x600;
		write32(ptr, val);
		ptr += 4;
	}
	val = (read32((MCTL_COM_BASE + MCTL_COM_WORK_MODE0)) & 0x1) ? 0x303 : 0x201;
	write32((MCTL_PHY_BASE + MCTL_PHY_ODTMAP), val);
	if(para->dram_para2 & (1 << 0))
		write32((MCTL_PHY_BASE + MCTL_PHY_DXnGCR0(1)), 0);
	if(para->dram_tpr4) {
		setbits_le32((MCTL_COM_BASE + MCTL_COM_WORK_MODE0), (para->dram_tpr4 & 0x3) << 25);
		setbits_le32((MCTL_COM_BASE + MCTL_COM_WORK_MODE1), (para->dram_tpr4 & 0x7fc) << 10);
	}
}

static void mctl_phy_ac_remapping(struct dram_param_t * para)
{
	uint8_t ac_remapping_tables[10][22];
	uint8_t * cfg = ac_remapping_tables[0];
	uint32_t fuse, val, chipid;

	ac_remapping_tables[0][0] = 0x00;
	ac_remapping_tables[0][1] = 0x00;
	ac_remapping_tables[0][2] = 0x00;
	ac_remapping_tables[0][3] = 0x00;
	ac_remapping_tables[0][4] = 0x00;
	ac_remapping_tables[0][5] = 0x00;
	ac_remapping_tables[0][6] = 0x00;
	ac_remapping_tables[0][7] = 0x00;
	ac_remapping_tables[0][8] = 0x00;
	ac_remapping_tables[0][9] = 0x00;
	ac_remapping_tables[0][10] = 0x00;
	ac_remapping_tables[0][11] = 0x00;
	ac_remapping_tables[0][12] = 0x00;
	ac_remapping_tables[0][13] = 0x00;
	ac_remapping_tables[0][14] = 0x00;
	ac_remapping_tables[0][15] = 0x00;
	ac_remapping_tables[0][16] = 0x00;
	ac_remapping_tables[0][17] = 0x00;
	ac_remapping_tables[0][18] = 0x00;
	ac_remapping_tables[0][19] = 0x00;
	ac_remapping_tables[0][20] = 0x00;
	ac_remapping_tables[0][21] = 0x00;

	ac_remapping_tables[1][0] = 0x01;
	ac_remapping_tables[1][1] = 0x09;
	ac_remapping_tables[1][2] = 0x03;
	ac_remapping_tables[1][3] = 0x07;
	ac_remapping_tables[1][4] = 0x08;
	ac_remapping_tables[1][5] = 0x12;
	ac_remapping_tables[1][6] = 0x04;
	ac_remapping_tables[1][7] = 0x0d;
	ac_remapping_tables[1][8] = 0x05;
	ac_remapping_tables[1][9] = 0x06;
	ac_remapping_tables[1][10] = 0x0a;
	ac_remapping_tables[1][11] = 0x02;
	ac_remapping_tables[1][12] = 0x0e;
	ac_remapping_tables[1][13] = 0x0c;
	ac_remapping_tables[1][14] = 0x00;
	ac_remapping_tables[1][15] = 0x00;
	ac_remapping_tables[1][16] = 0x15;
	ac_remapping_tables[1][17] = 0x11;
	ac_remapping_tables[1][18] = 0x14;
	ac_remapping_tables[1][19] = 0x13;
	ac_remapping_tables[1][20] = 0x0b;
	ac_remapping_tables[1][21] = 0x16;

	ac_remapping_tables[2][0] = 0x04;
	ac_remapping_tables[2][1] = 0x09;
	ac_remapping_tables[2][2] = 0x03;
	ac_remapping_tables[2][3] = 0x07;
	ac_remapping_tables[2][4] = 0x08;
	ac_remapping_tables[2][5] = 0x12;
	ac_remapping_tables[2][6] = 0x01;
	ac_remapping_tables[2][7] = 0x0d;
	ac_remapping_tables[2][8] = 0x02;
	ac_remapping_tables[2][9] = 0x06;
	ac_remapping_tables[2][10] = 0x0a;
	ac_remapping_tables[2][11] = 0x05;
	ac_remapping_tables[2][12] = 0x0e;
	ac_remapping_tables[2][13] = 0x0c;
	ac_remapping_tables[2][14] = 0x00;
	ac_remapping_tables[2][15] = 0x00;
	ac_remapping_tables[2][16] = 0x15;
	ac_remapping_tables[2][17] = 0x11;
	ac_remapping_tables[2][18] = 0x14;
	ac_remapping_tables[2][19] = 0x13;
	ac_remapping_tables[2][20] = 0x0b;
	ac_remapping_tables[2][21] = 0x16;

	ac_remapping_tables[3][0] = 0x01;
	ac_remapping_tables[3][1] = 0x07;
	ac_remapping_tables[3][2] = 0x08;
	ac_remapping_tables[3][3] = 0x0c;
	ac_remapping_tables[3][4] = 0x0a;
	ac_remapping_tables[3][5] = 0x12;
	ac_remapping_tables[3][6] = 0x04;
	ac_remapping_tables[3][7] = 0x0d;
	ac_remapping_tables[3][8] = 0x05;
	ac_remapping_tables[3][9] = 0x06;
	ac_remapping_tables[3][10] = 0x03;
	ac_remapping_tables[3][11] = 0x02;
	ac_remapping_tables[3][12] = 0x09;
	ac_remapping_tables[3][13] = 0x00;
	ac_remapping_tables[3][14] = 0x00;
	ac_remapping_tables[3][15] = 0x00;
	ac_remapping_tables[3][16] = 0x15;
	ac_remapping_tables[3][17] = 0x11;
	ac_remapping_tables[3][18] = 0x14;
	ac_remapping_tables[3][19] = 0x13;
	ac_remapping_tables[3][20] = 0x0b;
	ac_remapping_tables[3][21] = 0x16;

	ac_remapping_tables[4][0] = 0x04;
	ac_remapping_tables[4][1] = 0x0c;
	ac_remapping_tables[4][2] = 0x0a;
	ac_remapping_tables[4][3] = 0x07;
	ac_remapping_tables[4][4] = 0x08;
	ac_remapping_tables[4][5] = 0x12;
	ac_remapping_tables[4][6] = 0x01;
	ac_remapping_tables[4][7] = 0x0d;
	ac_remapping_tables[4][8] = 0x02;
	ac_remapping_tables[4][9] = 0x06;
	ac_remapping_tables[4][10] = 0x03;
	ac_remapping_tables[4][11] = 0x05;
	ac_remapping_tables[4][12] = 0x09;
	ac_remapping_tables[4][13] = 0x00;
	ac_remapping_tables[4][14] = 0x00;
	ac_remapping_tables[4][15] = 0x00;
	ac_remapping_tables[4][16] = 0x15;
	ac_remapping_tables[4][17] = 0x11;
	ac_remapping_tables[4][18] = 0x14;
	ac_remapping_tables[4][19] = 0x13;
	ac_remapping_tables[4][20] = 0x0b;
	ac_remapping_tables[4][21] = 0x16;

	ac_remapping_tables[5][0] = 0x0d;
	ac_remapping_tables[5][1] = 0x02;
	ac_remapping_tables[5][2] = 0x07;
	ac_remapping_tables[5][3] = 0x09;
	ac_remapping_tables[5][4] = 0x0c;
	ac_remapping_tables[5][5] = 0x13;
	ac_remapping_tables[5][6] = 0x05;
	ac_remapping_tables[5][7] = 0x01;
	ac_remapping_tables[5][8] = 0x06;
	ac_remapping_tables[5][9] = 0x03;
	ac_remapping_tables[5][10] = 0x04;
	ac_remapping_tables[5][11] = 0x08;
	ac_remapping_tables[5][12] = 0x0a;
	ac_remapping_tables[5][13] = 0x00;
	ac_remapping_tables[5][14] = 0x00;
	ac_remapping_tables[5][15] = 0x00;
	ac_remapping_tables[5][16] = 0x15;
	ac_remapping_tables[5][17] = 0x16;
	ac_remapping_tables[5][18] = 0x12;
	ac_remapping_tables[5][19] = 0x11;
	ac_remapping_tables[5][20] = 0x0b;
	ac_remapping_tables[5][21] = 0x14;

	ac_remapping_tables[6][0] = 0x03;
	ac_remapping_tables[6][1] = 0x0a;
	ac_remapping_tables[6][2] = 0x07;
	ac_remapping_tables[6][3] = 0x0d;
	ac_remapping_tables[6][4] = 0x09;
	ac_remapping_tables[6][5] = 0x0b;
	ac_remapping_tables[6][6] = 0x01;
	ac_remapping_tables[6][7] = 0x02;
	ac_remapping_tables[6][8] = 0x04;
	ac_remapping_tables[6][9] = 0x06;
	ac_remapping_tables[6][10] = 0x08;
	ac_remapping_tables[6][11] = 0x05;
	ac_remapping_tables[6][12] = 0x0c;
	ac_remapping_tables[6][13] = 0x00;
	ac_remapping_tables[6][14] = 0x00;
	ac_remapping_tables[6][15] = 0x00;
	ac_remapping_tables[6][16] = 0x14;
	ac_remapping_tables[6][17] = 0x12;
	ac_remapping_tables[6][18] = 0x00;
	ac_remapping_tables[6][19] = 0x15;
	ac_remapping_tables[6][20] = 0x16;
	ac_remapping_tables[6][21] = 0x11;

	ac_remapping_tables[7][0] = 0x03;
	ac_remapping_tables[7][1] = 0x02;
	ac_remapping_tables[7][2] = 0x04;
	ac_remapping_tables[7][3] = 0x07;
	ac_remapping_tables[7][4] = 0x09;
	ac_remapping_tables[7][5] = 0x01;
	ac_remapping_tables[7][6] = 0x11;
	ac_remapping_tables[7][7] = 0x0c;
	ac_remapping_tables[7][8] = 0x12;
	ac_remapping_tables[7][9] = 0x0e;
	ac_remapping_tables[7][10] = 0x0d;
	ac_remapping_tables[7][11] = 0x08;
	ac_remapping_tables[7][12] = 0x0f;
	ac_remapping_tables[7][13] = 0x06;
	ac_remapping_tables[7][14] = 0x0a;
	ac_remapping_tables[7][15] = 0x05;
	ac_remapping_tables[7][16] = 0x13;
	ac_remapping_tables[7][17] = 0x16;
	ac_remapping_tables[7][18] = 0x10;
	ac_remapping_tables[7][19] = 0x15;
	ac_remapping_tables[7][20] = 0x14;
	ac_remapping_tables[7][21] = 0x0b;

	ac_remapping_tables[8][0] = 0x02;
	ac_remapping_tables[8][1] = 0x13;
	ac_remapping_tables[8][2] = 0x08;
	ac_remapping_tables[8][3] = 0x06;
	ac_remapping_tables[8][4] = 0x0e;
	ac_remapping_tables[8][5] = 0x05;
	ac_remapping_tables[8][6] = 0x14;
	ac_remapping_tables[8][7] = 0x0a;
	ac_remapping_tables[8][8] = 0x03;
	ac_remapping_tables[8][9] = 0x12;
	ac_remapping_tables[8][10] = 0x0d;
	ac_remapping_tables[8][11] = 0x0b;
	ac_remapping_tables[8][12] = 0x07;
	ac_remapping_tables[8][13] = 0x0f;
	ac_remapping_tables[8][14] = 0x09;
	ac_remapping_tables[8][15] = 0x01;
	ac_remapping_tables[8][16] = 0x16;
	ac_remapping_tables[8][17] = 0x15;
	ac_remapping_tables[8][18] = 0x11;
	ac_remapping_tables[8][19] = 0x0c;
	ac_remapping_tables[8][20] = 0x04;
	ac_remapping_tables[8][21] = 0x10;

	ac_remapping_tables[9][0] = 0x01;
	ac_remapping_tables[9][1] = 0x02;
	ac_remapping_tables[9][2] = 0x0d;
	ac_remapping_tables[9][3] = 0x08;
	ac_remapping_tables[9][4] = 0x0f;
	ac_remapping_tables[9][5] = 0x0c;
	ac_remapping_tables[9][6] = 0x13;
	ac_remapping_tables[9][7] = 0x0a;
	ac_remapping_tables[9][8] = 0x03;
	ac_remapping_tables[9][9] = 0x15;
	ac_remapping_tables[9][10] = 0x06;
	ac_remapping_tables[9][11] = 0x11;
	ac_remapping_tables[9][12] = 0x09;
	ac_remapping_tables[9][13] = 0x0e;
	ac_remapping_tables[9][14] = 0x05;
	ac_remapping_tables[9][15] = 0x10;
	ac_remapping_tables[9][16] = 0x14;
	ac_remapping_tables[9][17] = 0x16;
	ac_remapping_tables[9][18] = 0x0b;
	ac_remapping_tables[9][19] = 0x07;
	ac_remapping_tables[9][20] = 0x04;
	ac_remapping_tables[9][21] = 0x12;

	if(para->dram_type != DRAM_TYPE_DDR2 && para->dram_type != DRAM_TYPE_DDR3)
		return;
	fuse = (read32(SYS_SID_BASE + SYS_EFUSE_REG) & 0xf00) >> 8;
	chipid = (read32(SYS_SID_BASE + SYS_CHIPID_REG) & 0xffff);
	if(para->dram_type == DRAM_TYPE_DDR2)
	{
		if(fuse == 15)
			return;
		if(fuse == 10)
			cfg = ac_remapping_tables[0];
		cfg = ac_remapping_tables[6];
	}
	else
	{
		if(para->dram_tpr13 & 0xc0000)
		{
			cfg = ac_remapping_tables[7];
		}
		else
		{
			if(fuse == 8)
			{
				cfg = ac_remapping_tables[2];
			}
			else if(fuse == 9)
			{
				cfg = ac_remapping_tables[3];
			}
			else if(fuse == 10)
			{
				if(chipid == 0x6800)
				{
					cfg = ac_remapping_tables[0];
				}
				else
				{
					cfg = ac_remapping_tables[5];
				}
			}
			else if(fuse == 11)
			{
				cfg = ac_remapping_tables[4];
			}
			else if(fuse == 12)
			{
				cfg = ac_remapping_tables[1];
			}
			else if(fuse == 13)
			{
				cfg = ac_remapping_tables[0];
			}
			else if(fuse == 14)
			{
				cfg = ac_remapping_tables[0];
			}
		}
	}
	val = (cfg[4] << 25) | (cfg[3] << 20) | (cfg[2] << 15) | (cfg[1] << 10) | (cfg[0] << 5);
	write32((MCTL_COM_BASE + MCTL_COM_REMAP0), val);
	val = (cfg[10] << 25) | (cfg[9] << 20) | (cfg[8] << 15) | (cfg[7] << 10) | (cfg[6] << 5) | cfg[5];
	write32((MCTL_COM_BASE + MCTL_COM_REMAP1), val);
	val = (cfg[15] << 20) | (cfg[14] << 15) | (cfg[13] << 10) | (cfg[12] << 5) | cfg[11];
	write32((MCTL_COM_BASE + MCTL_COM_REMAP2), val);
	val = (cfg[21] << 25) | (cfg[20] << 20) | (cfg[19] << 15) | (cfg[18] << 10) | (cfg[17] << 5) | cfg[16];
	write32((MCTL_COM_BASE + MCTL_COM_REMAP3), val);
	val = (cfg[4] << 25) | (cfg[3] << 20) | (cfg[2] << 15) | (cfg[1] << 10) | (cfg[0] << 5) | 1;
	write32((MCTL_COM_BASE + MCTL_COM_REMAP0), val);
}

static unsigned int mctl_channel_init(unsigned int ch_index, struct dram_param_t * para)
{
	unsigned int val, dqs_gating_mode;

	dqs_gating_mode = (para->dram_tpr13 & 0xc) >> 2;
	clrsetbits_le32((MCTL_COM_BASE + MCTL_COM_TMR), 0xfff, (para->dram_clk / 2) - 1);
	clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR2), 0xf00, 0x300);
	if(para->dram_odt_en)
		val = 0;
	else
		val = (1 << 5);
	if(para->dram_clk > 672)
		clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_DXnGCR0(0)), 0xf63e, val);
	else
		clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_DXnGCR0(0)), 0xf03e, val);
	if(para->dram_clk > 672)
	{
		setbits_le32((MCTL_PHY_BASE + MCTL_PHY_DXnGCR0(0)), 0x400);
		clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_DXnGCR0(1)), 0xf63e, val);
	}
	else
	{
		clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_DXnGCR0(1)), 0xf03e, val);
	}
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_ACIOCR0), (1 << 1));
	eye_delay_compensation(para);
	val = read32((MCTL_PHY_BASE + MCTL_PHY_PGCR2));
	if(dqs_gating_mode == 1)
	{
		clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR2), 0xc0, 0);
		clrbits_le32((MCTL_PHY_BASE + MCTL_PHY_DQSGMR), 0x107);
	}
	else if(dqs_gating_mode == 2)
	{
		clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR2), 0xc0, 0x80);
		clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_DQSGMR), 0x107, (((para->dram_tpr13 >> 16) & 0x1f) - 2) | 0x100);
		clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_DXCCR), (1 << 31), (1 << 27));
	}
	else
	{
		clrbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR2), 0x40);
		sdelay(10);
		setbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR2), 0xc0);
	}
	if(para->dram_type == DRAM_TYPE_LPDDR2 || para->dram_type == DRAM_TYPE_LPDDR3)
	{
		if(dqs_gating_mode == 1)
			clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_DXCCR), 0x080000c0, 0x80000000);
		else
			clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_DXCCR), 0x77000000, 0x22000000);
	}
	clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_DTCR), 0x0fffffff, (para->dram_para2 & (1 << 12)) ? 0x03000001 : 0x01000007);
	if(read32((R_CPUCFG_BASE + R_CPUCFG_SUP_STAN_FLAG)) & (1 << 16))
	{
		clrbits_le32((R_PRCM_BASE + VDD_SYS_PWROFF_GATING), 0x2);
		sdelay(10);
	}
	clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_ZQCR), 0x3ffffff, (para->dram_zq & 0x00ffffff) | (1 << 25));
	if(dqs_gating_mode == 1)
	{
		write32((MCTL_PHY_BASE + MCTL_PHY_PIR), 0x53);
		while((read32((MCTL_PHY_BASE + MCTL_PHY_PGSR0)) & 0x1) == 0);
		sdelay(10);
		if(para->dram_type == DRAM_TYPE_DDR3)
			write32((MCTL_PHY_BASE + MCTL_PHY_PIR), 0x5a0);
		else
			write32((MCTL_PHY_BASE + MCTL_PHY_PIR), 0x520);
	}
	else
	{
		if((read32((R_CPUCFG_BASE + R_CPUCFG_SUP_STAN_FLAG)) & (1 << 16)) == 0)
		{
			if(para->dram_type == DRAM_TYPE_DDR3)
				write32((MCTL_PHY_BASE + MCTL_PHY_PIR), 0x1f2);
			else
				write32((MCTL_PHY_BASE + MCTL_PHY_PIR), 0x172);
		}
		else
		{
			write32((MCTL_PHY_BASE + MCTL_PHY_PIR), 0x62);
		}
	}
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_PIR), 0x1);
	sdelay(10);
	while((read32((MCTL_PHY_BASE + MCTL_PHY_PGSR0)) & 0x1) == 0);
	if(read32((R_CPUCFG_BASE + R_CPUCFG_SUP_STAN_FLAG)) & (1 << 16))
	{
		clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR3), 0x06000000, 0x04000000);
		sdelay(10);
		setbits_le32((MCTL_PHY_BASE + MCTL_PHY_PWRCTL), 0x1);
		while((read32((MCTL_PHY_BASE + MCTL_PHY_STATR)) & 0x7) != 0x3);
		clrbits_le32((R_PRCM_BASE + VDD_SYS_PWROFF_GATING), 0x1);
		sdelay(10);
		clrbits_le32((MCTL_PHY_BASE + MCTL_PHY_PWRCTL), 0x1);
		while ((read32((MCTL_PHY_BASE + MCTL_PHY_STATR)) & 0x7) != 0x1);
		sdelay(15);
		if(dqs_gating_mode == 1)
		{
			clrbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR2), 0xc0);
			clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR3), 0x06000000, 0x02000000);
			sdelay(1);
			write32((MCTL_PHY_BASE + MCTL_PHY_PIR), 0x401);
			while((read32((MCTL_PHY_BASE + MCTL_PHY_PGSR0)) & 0x1) == 0);
		}
	}
	if(read32((MCTL_PHY_BASE + MCTL_PHY_PGSR0)) & (1 << 20))
		return 0;
	while((read32((MCTL_PHY_BASE + MCTL_PHY_STATR)) & 0x1) == 0);
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_RFSHCTL0), (1 << 31));
	sdelay(10);
	clrbits_le32((MCTL_PHY_BASE + MCTL_PHY_RFSHCTL0), (1 << 31));
	sdelay(10);
	setbits_le32((MCTL_COM_BASE + MCTL_COM_CCCR), (1 << 31));
	sdelay(10);
	clrbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR3), 0x06000000);
	if(dqs_gating_mode == 1)
		clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_DXCCR), 0xc0, 0x40);
	return 1;
}

static unsigned int calculate_rank_size(uint32_t regval)
{
	unsigned int bits;

	bits = (regval >> 8) & 0xf;
	bits += (regval >> 4) & 0xf;
	bits += (regval >> 2) & 0x3;
	bits -= 14;
	return 1U << bits;
}

static unsigned int dramc_get_dram_size(void)
{
	uint32_t val;
	unsigned int size;

	val = read32((MCTL_COM_BASE + MCTL_COM_WORK_MODE0));
	size = calculate_rank_size(val);
	if((val & 0x3) == 0)
		return size;
	val = read32((MCTL_COM_BASE + MCTL_COM_WORK_MODE1));
	if((val & 0x3) == 0)
		return size * 2;
	return size + calculate_rank_size(val);
}

static int dqs_gate_detect(struct dram_param_t * para)
{
	uint32_t dx0 = 0, dx1 = 0;

	if((read32(MCTL_PHY_BASE + MCTL_PHY_PGSR0) & (1 << 22)) == 0)
	{
		para->dram_para2 = (para->dram_para2 & ~0xf) | (1 << 12);
		return 1;
	}
	dx0 = (read32(MCTL_PHY_BASE + MCTL_PHY_DXnGSR0(0)) & 0x3000000) >> 24;
	if(dx0 == 0)
	{
		para->dram_para2 = (para->dram_para2 & ~0xf) | 0x1001;
		return 1;
	}
	if(dx0 == 2)
	{
		dx1 = (read32(MCTL_PHY_BASE + MCTL_PHY_DXnGSR0(1)) & 0x3000000) >> 24;
		if(dx1 == 2)
			para->dram_para2 = para->dram_para2 & ~0xf00f;
		else
			para->dram_para2 = (para->dram_para2 & ~0xf00f) | (1 << 0);
		return 1;
	}
	if((para->dram_tpr13 & (1 << 29)) == 0)
		return 0;
	return 0;
}

static int dramc_simple_wr_test(unsigned int mem_mb, int len)
{
	unsigned int offs = (mem_mb / 2) << 18;
	unsigned int patt1 = 0x01234567;
	unsigned int patt2 = 0xfedcba98;
	unsigned int * addr, v1, v2, i;

	addr = (unsigned int *)CONFIG_DRAM_BASE;
	for(i = 0; i != len; i++, addr++)
	{
		write32((unsigned long)addr, patt1 + i);
		write32((unsigned long)(addr + offs), patt2 + i);
	}
	addr = (unsigned int *)CONFIG_DRAM_BASE;
	for(i = 0; i != len; i++)
	{
		v1 = read32((unsigned long)(addr + i));
		v2 = patt1 + i;
		if(v1 != v2)
			return 1;
		v1 = read32((unsigned long)(addr + offs + i));
		v2 = patt2 + i;
		if(v1 != v2)
			return 1;
	}
	return 0;
}

static void mctl_vrefzq_init(struct dram_param_t * para)
{
	if(para->dram_tpr13 & (1 << 17))
		return;
	clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_IOVCR0), 0x7f7f7f7f, para->dram_tpr5);
	if((para->dram_tpr13 & (1 << 16)) == 0)
		clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_IOVCR1), 0x7f, para->dram_tpr6 & 0x7f);
}

static int mctl_core_init(struct dram_param_t * para)
{
	mctl_sys_init(para);
	mctl_vrefzq_init(para);
	mctl_com_init(para);
	mctl_phy_ac_remapping(para);
	mctl_set_timing_params(para);
	return mctl_channel_init(0, para);
}

static int auto_scan_dram_size(struct dram_param_t *para)
{
	uint32_t i = 0, j = 0, current_rank = 0;
	uint32_t rank_count = 1, addr_line = 0;
	uint32_t reg_val = 0, ret = 0, cnt = 0;
	unsigned long mc_work_mode;
	uint32_t rank1_addr = CONFIG_DRAM_BASE;

	if(mctl_core_init(para) == 0)
		return 0;
	if((((para->dram_para2 >> 12) & 0xf) == 0x1))
		rank_count = 2;
	for(current_rank = 0; current_rank < rank_count; current_rank++)
	{
		mc_work_mode = ((MCTL_COM_BASE + MCTL_COM_WORK_MODE0) + 4 * current_rank);
		if(current_rank == 1)
		{
			clrsetbits_le32((MCTL_COM_BASE + MCTL_COM_WORK_MODE0), 0xf0c, 0x6f0);
			clrsetbits_le32((MCTL_COM_BASE + MCTL_COM_WORK_MODE1), 0xf0c, 0x6f0);
			rank1_addr = CONFIG_DRAM_BASE + (0x1 << 27);
		}
		for(i = 0; i < 64; i++)
			write32(CONFIG_DRAM_BASE + 4 * i, (i % 2) ? (CONFIG_DRAM_BASE + 4 * i) : (~(CONFIG_DRAM_BASE + 4 * i)));
		clrsetbits_le32(mc_work_mode, 0xf0c, 0x6f0);
		sdelay(2);
		for(i = 11; i < 17; i++)
		{
			ret = CONFIG_DRAM_BASE + (1 << (i + 2 + 9));
			cnt = 0;
			for(j = 0; j < 64; j++)
			{
				reg_val = (j % 2) ? (rank1_addr + 4 * j) : (~(rank1_addr + 4 * j));
				if(reg_val == read32(ret + j * 4))
					cnt++;
				else
					break;
			}
			if(cnt == 64)
				break;
		}
		if(i >= 16)
			i = 16;
		addr_line += i;
		para->dram_para1 &= ~(0xffU << (16 * current_rank + 4));
		para->dram_para1 |= (i << (16 * current_rank + 4));
		if(current_rank == 1)
			clrsetbits_le32((MCTL_COM_BASE + MCTL_COM_WORK_MODE0), 0xffc, 0x6a4);
		clrsetbits_le32(mc_work_mode, 0xffc, 0x6a4);
		sdelay(1);
		for(i = 0; i < 1; i++)
		{
			ret = CONFIG_DRAM_BASE + (0x1U << (i + 2 + 9));
			cnt = 0;
			for(j = 0; j < 64; j++)
			{
				reg_val = (j % 2) ? (rank1_addr + 4 * j) : (~(rank1_addr + 4 * j));
				if(reg_val == read32(ret + j * 4))
					cnt++;
				else
					break;
			}
			if(cnt == 64)
				break;
		}
		addr_line += i + 2;
		para->dram_para1 &= ~(0xfU << (16 * current_rank + 12));
		para->dram_para1 |= (i << (16 * current_rank + 12));
		if(current_rank == 1)
			clrsetbits_le32(mc_work_mode, 0xffc, 0xaa0);
		clrsetbits_le32(mc_work_mode, 0xffc, 0xaa0);
		sdelay(2);
		for(i = 9; i <= 13; i++)
		{
			ret = CONFIG_DRAM_BASE + (0x1U << i);
			cnt = 0;
			for(j = 0; j < 64; j++)
			{
				reg_val = (j % 2) ? (CONFIG_DRAM_BASE + 4 * j) : (~(CONFIG_DRAM_BASE + 4 * j));
				if(reg_val == read32(ret + j * 4))
					cnt++;
				else
					break;
			}
			if(cnt == 64)
				break;
		}
		if(i >= 13)
			i = 13;
		addr_line += i;
		if(i == 9)
			i = 0;
		else
			i = (0x1U << (i - 10));
		para->dram_para1 &= ~(0xfU << (16 * current_rank));
		para->dram_para1 |= (i << (16 * current_rank));
	}
	if(rank_count == 2)
	{
		para->dram_para2 &= 0xfffff0ff;
		if((para->dram_para1 & 0xffff) == (para->dram_para1 >> 16))
		{
		}
		else
		{
			para->dram_para2 |= 0x1 << 8;
		}
	}
	return 1;
}

static int auto_scan_dram_rank_width(struct dram_param_t * para)
{
	unsigned int s1 = para->dram_tpr13;
	unsigned int s2 = para->dram_para1;

	para->dram_para1 = 0x00b000b0;
	para->dram_para2 = (para->dram_para2 & ~0xf) | (1 << 12);
	para->dram_tpr13 = (para->dram_tpr13 & ~0x8) | (1 << 2) | (1 << 0);
	mctl_core_init(para);
	if(read32((MCTL_PHY_BASE + MCTL_PHY_PGSR0)) & (1 << 20))
		return 0;
	if(dqs_gate_detect(para) == 0)
		return 0;
	para->dram_tpr13 = s1;
	para->dram_para1 = s2;
	return 1;
}

static int auto_scan_dram_config(struct dram_param_t *para)
{
	if(((para->dram_tpr13 & (1 << 14)) == 0) && (auto_scan_dram_rank_width(para) == 0))
		return 0;
	if(((para->dram_tpr13 & (1 << 0)) == 0) && (auto_scan_dram_size(para) == 0))
		return 0;
	if((para->dram_tpr13 & (1 << 15)) == 0)
		para->dram_tpr13 |= (1 << 14) | (1 << 13) | (1 << 1) | (1 << 0);
	return 1;
}

static int init_dram(struct dram_param_t * para)
{
	uint32_t rc, mem_size_mb;

	if(para->dram_tpr13 & (1 << 16))
	{
		setbits_le32((SYS_CONTROL_REG_BASE + ZQ_CAL_CTRL_REG), (1 << 8));
		write32((SYS_CONTROL_REG_BASE + ZQ_RES_CTRL_REG), 0);
		sdelay(10);
	}
	else
	{
		clrbits_le32((SYS_CONTROL_REG_BASE + ZQ_CAL_CTRL_REG), 0x3);
		write32((R_PRCM_BASE + ANALOG_PWROFF_GATING), para->dram_tpr13 & (1 << 16));
		sdelay(10);
		clrsetbits_le32((SYS_CONTROL_REG_BASE + ZQ_CAL_CTRL_REG), 0x108, (1 << 1));
		sdelay(10);
		setbits_le32((SYS_CONTROL_REG_BASE + ZQ_CAL_CTRL_REG), (1 << 0));
		sdelay(20);
	}
	dram_voltage_set(para);
	if((para->dram_tpr13 & (1 << 0)) == 0)
	{
		if(auto_scan_dram_config(para) == 0)
			return 0;
	}
	rc = para->dram_mr1;
	if(mctl_core_init(para) == 0)
		return 0;
	rc = para->dram_para2;
	if(rc & (1 << 31))
	{
		rc = (rc >> 16) & ~(1 << 15);
	}
	else
	{
		rc = dramc_get_dram_size();
		para->dram_para2 = (para->dram_para2 & 0xffffU) | rc << 16;
	}
	mem_size_mb = rc;
	if(para->dram_tpr13 & (1 << 30))
	{
		rc = para->dram_tpr8;
		if(rc == 0)
			rc = 0x10000200;
		write32((MCTL_PHY_BASE + MCTL_PHY_ASRTC), rc);
		write32((MCTL_PHY_BASE + MCTL_PHY_ASRC), 0x40a);
		setbits_le32((MCTL_PHY_BASE + MCTL_PHY_PWRCTL), (1 << 0));
	}
	else
	{
		clrbits_le32((MCTL_PHY_BASE + MCTL_PHY_ASRTC), 0xffff);
		clrbits_le32((MCTL_PHY_BASE + MCTL_PHY_PWRCTL), 0x1);
	}
	if(para->dram_tpr13 & (1 << 9))
	{
		clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR0), 0xf000, 0x5000);
	}
	else
	{
		if(para->dram_type != DRAM_TYPE_LPDDR2)
			clrbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR0), 0xf000);
	}
	setbits_le32((MCTL_PHY_BASE + MCTL_PHY_ZQCR), (1 << 31));
	if(para->dram_tpr13 & (1 << 8))
		write32((MCTL_PHY_BASE + MCTL_PHY_VTFCR), read32((MCTL_PHY_BASE + MCTL_PHY_VTFCR)) | 0x300);
	if(para->dram_tpr13 & (1 << 16))
		clrbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR2), (1 << 13));
	else
		setbits_le32((MCTL_PHY_BASE + MCTL_PHY_PGCR2), (1 << 13));
	if(para->dram_type == DRAM_TYPE_LPDDR3)
		clrsetbits_le32((MCTL_PHY_BASE + MCTL_PHY_ODTCFG), 0xf0000, 0x1000);
	dram_enable_all_master();
	if(para->dram_tpr13 & (1 << 28))
	{
		if((read32((R_CPUCFG_BASE + R_CPUCFG_SUP_STAN_FLAG)) & (1 << 16)) || dramc_simple_wr_test(mem_size_mb, 4096))
			return 0;
	}
	return mem_size_mb;
}

void sys_dram_init(void)
{
	struct dram_param_t para = {
		.dram_clk = 792,
		.dram_type = 3,
		.dram_zq = 0x7b7bfb,
		.dram_odt_en = 0x00,
		.dram_para1 = 0x000010d2,
		.dram_para2 = 0x0000,
		.dram_mr0 = 0x1c70,
		.dram_mr1 = 0x042,
		.dram_mr2 = 0x18,
		.dram_mr3 = 0x0,
		.dram_tpr0 = 0x004A2195,
		.dram_tpr1 = 0x02423190,
		.dram_tpr2 = 0x0008B061,
		.dram_tpr3 = 0xB4787896,
		.dram_tpr4 = 0x0,
		.dram_tpr5 = 0x48484848,
		.dram_tpr6 = 0x00000048,
		.dram_tpr7 = 0x1620121e,
		.dram_tpr8 = 0x0,
		.dram_tpr9 = 0x0,
		.dram_tpr10 = 0x0,
		.dram_tpr11 = 0x00340000,
		.dram_tpr12 = 0x00000046,
		.dram_tpr13 = 0x34000100,
	};
	init_dram(&para);
}
