#include <xboot.h>
#include <clk/clk.h>
#include <uart/uart.h>
#include <gx6605s-cir.h>

struct cir_gx6605s_pdata_t {
	virtual_addr_t virt;
	char * clk;
};

static int cir_gx6605s_poll(struct timer_t * timer, void * data)
{
	struct cir_gx6605s_pdata_t * pdat = (struct cir_gx6605s_pdata_t *)data;
	u32_t val;

	val = read32(pdat->virt + GX6605S_CIR_INT);
	if (val & GX6605S_INT_FIFOTS) {
		int num = (val & GX6605S_INT_NUM) >> 3;
		int i;
		for (i = 0; i < num; i++) {
			u32_t code = read32(pdat->virt + GX6605S_CIR_FIFO);
			printf("CIR RX: 0x%08x\r\n", code);
		}
		write32(pdat->virt + GX6605S_CIR_INT, val); /* clear interrupt */
	}

	timer_forward(timer, ms_to_ktime(100));
	return 1;
}

static struct device_t * cir_gx6605s_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct cir_gx6605s_pdata_t * pdat;
	struct device_t * dev;
	struct timer_t * timer;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * clk = dt_read_string(n, "clock-name", NULL);

	if(!search_clk(clk))
		return NULL;

	pdat = malloc(sizeof(struct cir_gx6605s_pdata_t));
	if(!pdat)
		return NULL;

	pdat->virt = virt;
	pdat->clk = strdup(clk);

	clk_enable(pdat->clk);

	/* initialize CIR */
	write32(pdat->virt + GX6605S_CIR_CLK, clk_get_rate(pdat->clk) / 1000000 - 1);
	write32(pdat->virt + GX6605S_CIR_CNTL, GX6605S_CIR_CNTL_TOV | GX6605S_CIR_CNTL_ENIR | GX6605S_CIR_CNTL_FIFOTM);

	dev = malloc(sizeof(struct device_t));
	if(!dev)
	{
		clk_disable(pdat->clk);
		free(pdat->clk);
		free(pdat);
		return NULL;
	}

	dev->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	dev->type = DEVICE_TYPE_INPUT; // generic, or maybe we just don't register it properly if we only poll
	dev->driver = drv;
	dev->priv = pdat;
	dev->kobj = kobj_alloc_directory(dev->name);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free_device_name(dev->name);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free(pdat);
		free(dev);
		return NULL;
	}

	timer = malloc(sizeof(struct timer_t));
	timer_init(timer, cir_gx6605s_poll, pdat);
	timer_start(timer, ms_to_ktime(100));

	return dev;
}

static void cir_gx6605s_remove(struct device_t * dev)
{
	struct cir_gx6605s_pdata_t * pdat = (struct cir_gx6605s_pdata_t *)dev->priv;

	if(dev)
	{
		unregister_device(dev);
		kobj_remove_self(dev->kobj);
		free_device_name(dev->name);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free(pdat);
		free(dev);
	}
}

static void cir_gx6605s_suspend(struct device_t * dev)
{
}

static void cir_gx6605s_resume(struct device_t * dev)
{
}

static struct driver_t cir_gx6605s = {
	.name		= "cir-gx6605s",
	.probe		= cir_gx6605s_probe,
	.remove		= cir_gx6605s_remove,
	.suspend	= cir_gx6605s_suspend,
	.resume		= cir_gx6605s_resume,
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
