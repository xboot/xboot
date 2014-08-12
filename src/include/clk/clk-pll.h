#ifndef __CLK_PLL_H__
#define __CLK_PLL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct clk_pll_t {
	const char * name;
	const char * parent;
	void (*set_rate)(struct clk_pll_t * pclk, u64_t prate, u64_t rate);
	u64_t (*get_rate)(struct clk_pll_t * pclk, u64_t prate);
};

bool_t clk_pll_register(struct clk_pll_t * pclk);
bool_t clk_pll_unregister(struct clk_pll_t * pclk);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_PLL_H__ */
