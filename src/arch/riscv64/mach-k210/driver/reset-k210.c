/*
 * driver/reset-k210.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <reset/reset.h>

struct reset_k210_pdata_t
{
	virtual_addr_t virt;
	int base;
	int nreset;
};

static void reset_k210_assert(struct resetchip_t * chip, int offset)
{
	struct reset_k210_pdata_t * pdat = (struct reset_k210_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->nreset)
		return;

	val = read32(pdat->virt);
	val |= (1 << offset);
	write32(pdat->virt, val);
}

static void reset_k210_deassert(struct resetchip_t * chip, int offset)
{
	struct reset_k210_pdata_t * pdat = (struct reset_k210_pdata_t *)chip->priv;
	u32_t val;

	if(offset >= chip->nreset)
		return;

	val = read32(pdat->virt);
	val &= ~(1 << offset);
	write32(pdat->virt, val);
}

static struct device_t * reset_k210_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct reset_k210_pdata_t * pdat;
	struct resetchip_t * chip;
	struct device_t * dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	int base = dt_read_int(n, "reset-base", -1);
	int nreset = dt_read_int(n, "reset-count", -1);

	if((base < 0) || (nreset <= 0))
		return NULL;

	pdat = malloc(sizeof(struct reset_k210_pdata_t));
	if(!pdat)
		return NULL;

	chip = malloc(sizeof(struct resetchip_t));
	if(!chip)
	{
		free(pdat);
		return NULL;
	}

	pdat->virt = virt;
	pdat->base = base;
	pdat->nreset = nreset;

	chip->name = alloc_device_name(dt_read_name(n), -1);
	chip->base = pdat->base;
	chip->nreset = pdat->nreset;
	chip->assert = reset_k210_assert;
	chip->deassert = reset_k210_deassert;
	chip->priv = pdat;

	if(!register_resetchip(&dev, chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void reset_k210_remove(struct device_t * dev)
{
	struct resetchip_t * chip = (struct resetchip_t *)dev->priv;

	if(chip && unregister_resetchip(chip))
	{
		free_device_name(chip->name);
		free(chip->priv);
		free(chip);
	}
}

static void reset_k210_suspend(struct device_t * dev)
{
}

static void reset_k210_resume(struct device_t * dev)
{
}

static struct driver_t reset_k210 = {
	.name		= "reset-k210",
	.probe		= reset_k210_probe,
	.remove		= reset_k210_remove,
	.suspend	= reset_k210_suspend,
	.resume		= reset_k210_resume,
};

static __init void reset_k210_driver_init(void)
{
	register_driver(&reset_k210);
}

static __exit void reset_k210_driver_exit(void)
{
	unregister_driver(&reset_k210);
}

driver_initcall(reset_k210_driver_init);
driver_exitcall(reset_k210_driver_exit);
