/*
 * GX6605S CIR Driver — FINAL FIXED VERSION
 *
 * - Uses FIFO NUM instead of FIFOTS
 * - Decodes pulse-width (NEC-like)
 * - Handles continuous stream (not per-burst)
 * - Stable frame reconstruction (32-bit)
 */

#include <xboot.h>
#include <clk/clk.h>
#include <uart/uart.h>
#include <input/input.h>
#include <input/keyboard.h>
#include <gx6605s-cir.h>

struct cir_gx6605s_pdata_t {
	virtual_addr_t   virt;
	char *           clk;
	struct input_t * input;

	u32_t frame;
	int   bit_count;
};

/* ---------------------------------------------------------- */
/* Decode ONE FIFO word                                       */
/* ---------------------------------------------------------- */
static void cir_process_word(struct cir_gx6605s_pdata_t * pdat, u32_t code)
{
	u8_t  top   = (code >> 24) & 0xff;
	u16_t space = (code >> 0)  & 0xffff;

	/* ---------------- HEADER ---------------- */
	if (top >= 0x13)
	{
		pdat->frame = 0;
		pdat->bit_count = 0;
		return;
	}

	/* ---------------- END ---------------- */
	if (top >= 0x0b)
	{
		if (pdat->bit_count == 32)
		{
			u8_t addr = (pdat->frame >> 0) & 0xff;
			u8_t cmd  = (pdat->frame >> 16) & 0xff;

			printf("CIR FRAME: addr=0x%02x cmd=0x%02x raw=0x%08x\r\n",
			       addr, cmd, pdat->frame);

			/* Example key mapping (fill later) */
			switch(cmd)
{
	case 0xDA: /* OK */
		push_event_key_down(pdat->input, KEY_ENTER);
		push_event_key_up(pdat->input, KEY_ENTER);
		break;

	case 0x8E: /* 1 */
		push_event_key_down(pdat->input, KEY_1);
		push_event_key_up(pdat->input, KEY_1);
		break;

	case 0x86: /* 2 */
		push_event_key_down(pdat->input, KEY_2);
		push_event_key_up(pdat->input, KEY_2);
		break;

	case 0x8F: /* 3 */
		push_event_key_down(pdat->input, KEY_3);
		push_event_key_up(pdat->input, KEY_3);
		break;

	case 0xD6: /* DOWN */
		push_event_key_down(pdat->input, KEY_DOWN);
		push_event_key_up(pdat->input, KEY_DOWN);
		break;

	case 0xDB: /* LEFT */
		push_event_key_down(pdat->input, KEY_LEFT);
		push_event_key_up(pdat->input, KEY_LEFT);
		break;

	case 0xD8: /* RIGHT */
		push_event_key_down(pdat->input, KEY_RIGHT);
		push_event_key_up(pdat->input, KEY_RIGHT);
		break;

	case 0xDE: /* TOP (UP) */
		push_event_key_down(pdat->input, KEY_UP);
		push_event_key_up(pdat->input, KEY_UP);
		break;

	default:
		printf("UNKNOWN KEY cmd=0x%02x\r\n", cmd);
		break;
}
		}

		pdat->frame = 0;
		pdat->bit_count = 0;
		return;
	}

	/* ---------------- DATA BIT ---------------- */
	int bit;

	if (space < 800)
		bit = 0;
	else if (space > 1200)
		bit = 1;
	else
		return; /* ignore noise */

	if (pdat->bit_count < 32)
	{
		pdat->frame |= (bit << pdat->bit_count);
		pdat->bit_count++;
	}
}

/* ---------------------------------------------------------- */
/* Poll (Timer-based)                                         */
/* ---------------------------------------------------------- */
static int cir_gx6605s_poll(struct timer_t * timer, void * data)
{
	struct cir_gx6605s_pdata_t * pdat = data;
	u32_t val = read32(pdat->virt + GX6605S_CIR_INT);

	int num = (val & GX6605S_INT_NUM) >> 3;

	/* 🔥 CRITICAL: use NUM, NOT FIFOTS */
	if (num > 0)
	{
		for (int i = 0; i < num; i++)
		{
			u32_t code = read32(pdat->virt + GX6605S_CIR_FIFO);
			cir_process_word(pdat, code);
		}

		/* clear interrupt */
		write32(pdat->virt + GX6605S_CIR_INT, val);
	}

	timer_forward(timer, ms_to_ktime(20));
	return 1;
}

/* ---------------------------------------------------------- */
/* IOCTL                                                      */
/* ---------------------------------------------------------- */
static int cir_gx6605s_ioctl(struct input_t * input, const char * cmd, void * arg)
{
	return -1;
}

/* ---------------------------------------------------------- */
/* Probe                                                      */
/* ---------------------------------------------------------- */
static struct device_t * cir_gx6605s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct cir_gx6605s_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	struct timer_t * timer;

	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if (!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct cir_gx6605s_pdata_t));
	if (!pdat)
		return NULL;

	pdat->virt = virt;
	pdat->clk  = strdup(clk);
	pdat->frame = 0;
	pdat->bit_count = 0;

	clk_enable(pdat->clk);

	u32_t rate = clk_get_rate(pdat->clk);

	/* 1 tick = 1µs */
	write32(pdat->virt + GX6605S_CIR_CLK, rate / 1000000 - 1);

	write32(pdat->virt + GX6605S_CIR_CNTL,
		GX6605S_CIR_CNTL_TOV  |
		GX6605S_CIR_CNTL_ENIR |
		GX6605S_CIR_CNTL_FIFOTM);

	printf("CIR INIT DONE (clk=%u Hz)\r\n", rate);

	input = malloc(sizeof(struct input_t));
	if (!input)
		return NULL;

	input->name  = alloc_device_name(dt_read_name(n), dt_read_id(n));
	input->ioctl = cir_gx6605s_ioctl;
	input->priv  = pdat;

	pdat->input = input;

	if (!(dev = register_input(input, drv)))
		return NULL;

	timer = malloc(sizeof(struct timer_t));
	timer_init(timer, cir_gx6605s_poll, pdat);
	timer_start(timer, ms_to_ktime(20));

	printf("CIR READY — press remote\r\n");

	return dev;
}

/* ---------------------------------------------------------- */
/* Remove                                                     */
/* ---------------------------------------------------------- */
static void cir_gx6605s_remove(struct device_t * dev)
{
	struct input_t * input = dev->priv;
	struct cir_gx6605s_pdata_t * pdat = input->priv;

	unregister_input(input);
	free_device_name(input->name);

	clk_disable(pdat->clk);
	free(pdat->clk);
	free(pdat);
	free(input);
}

static void cir_gx6605s_suspend(struct device_t * dev) {}
static void cir_gx6605s_resume(struct device_t * dev) {}

/* ---------------------------------------------------------- */
/* Driver                                                     */
/* ---------------------------------------------------------- */
static struct driver_t cir_gx6605s = {
	.name    = "cir-gx6605s",
	.probe   = cir_gx6605s_probe,
	.remove  = cir_gx6605s_remove,
	.suspend = cir_gx6605s_suspend,
	.resume  = cir_gx6605s_resume,
};

static __init void cir_gx6605s_driver_init(void)
{
	register_driver(&cir_gx6605s);
}

static __exit void cir_gx6605s_driver_exit(void)
{
	unregister_driver(&cir_gx6605s);
}

driver_initcall(cir_gx6605s_driver_init);
driver_exitcall(cir_gx6605s_driver_exit);